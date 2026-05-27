import { mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";

type PlistValue = null | boolean | number | string | PlistValue[] | PlistDict;
type PlistDict = { [key: string]: PlistValue };

type Token =
  | { kind: "open"; name: "dict" | "array" }
  | { kind: "close"; name: "dict" | "array" }
  | { kind: "key"; value: string }
  | { kind: "value"; value: PlistValue };

type CcbProperty = {
  name: string;
  type: string;
  value: PlistValue;
  baseValue?: PlistValue;
};

type CcbNode = {
  id: number;
  path: string;
  depth: number;
  parentId: number | null;
  baseClass: string;
  displayName: string;
  properties: CcbProperty[];
  propertyMap: Map<string, CcbProperty>;
  animatedProperties: PlistDict;
  children: CcbNode[];
};

type CcbSequence = {
  sequenceId: number;
  name: string;
  length: number;
  resolution: number;
  autoPlay: boolean;
  chainedSequenceId: number;
  offset?: number;
  position?: number;
  scale?: number;
};

type AnalyzerResult = {
  root: PlistDict;
  nodes: CcbNode[];
  sequences: CcbSequence[];
};

const inputPath = process.argv[2] ?? "data/animal_horse/node_animal_horse.ccb";
const outputDir = process.argv[3] ?? "analysis/animal_horse";

function decodeXml(text: string): string {
  return text
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'")
    .replace(/&amp;/g, "&");
}

function tokenizePlist(xml: string): Token[] {
  const tokens: Token[] = [];
  const pattern =
    /<(dict|array)\s*\/>|<(dict|array)>|<\/(dict|array)>|<key>([\s\S]*?)<\/key>|<string>([\s\S]*?)<\/string>|<integer>([\s\S]*?)<\/integer>|<real>([\s\S]*?)<\/real>|<(true|false)\s*\/>/g;

  for (const match of xml.matchAll(pattern)) {
    if (match[1] === "dict") {
      tokens.push({ kind: "value", value: {} });
    } else if (match[1] === "array") {
      tokens.push({ kind: "value", value: [] });
    } else if (match[2] === "dict" || match[2] === "array") {
      tokens.push({ kind: "open", name: match[2] });
    } else if (match[3] === "dict" || match[3] === "array") {
      tokens.push({ kind: "close", name: match[3] });
    } else if (match[4] !== undefined) {
      tokens.push({ kind: "key", value: decodeXml(match[4]) });
    } else if (match[5] !== undefined) {
      tokens.push({ kind: "value", value: decodeXml(match[5]) });
    } else if (match[6] !== undefined) {
      tokens.push({ kind: "value", value: Number.parseInt(match[6], 10) });
    } else if (match[7] !== undefined) {
      tokens.push({ kind: "value", value: Number.parseFloat(match[7]) });
    } else if (match[8] !== undefined) {
      tokens.push({ kind: "value", value: match[8] === "true" });
    }
  }

  return tokens;
}

function parsePlist(xml: string): PlistValue {
  const tokens = tokenizePlist(xml);
  let index = 0;

  function next(): Token {
    const token = tokens[index++];
    if (!token) {
      throw new Error("Unexpected end of plist");
    }
    return token;
  }

  function parseValue(): PlistValue {
    const token = next();
    if (token.kind === "value") {
      return token.value;
    }

    if (token.kind !== "open") {
      throw new Error(`Expected plist value, got ${token.kind}`);
    }

    if (token.name === "array") {
      const values: PlistValue[] = [];
      while (tokens[index]?.kind !== "close") {
        values.push(parseValue());
      }
      const close = next();
      if (close.kind !== "close" || close.name !== "array") {
        throw new Error("Array close token mismatch");
      }
      return values;
    }

    const dict: PlistDict = {};
    while (tokens[index]?.kind !== "close") {
      const key = next();
      if (key.kind !== "key") {
        throw new Error(`Expected dict key, got ${key.kind}`);
      }
      dict[key.value] = parseValue();
    }
    const close = next();
    if (close.kind !== "close" || close.name !== "dict") {
      throw new Error("Dict close token mismatch");
    }
    return dict;
  }

  return parseValue();
}

function asDict(value: PlistValue | undefined, label: string): PlistDict {
  if (!value || Array.isArray(value) || typeof value !== "object") {
    throw new Error(`Expected ${label} to be a dictionary`);
  }
  return value;
}

function asArray(value: PlistValue | undefined): PlistValue[] {
  return Array.isArray(value) ? value : [];
}

function asNumber(value: PlistValue | undefined, fallback = 0): number {
  return typeof value === "number" ? value : fallback;
}

function asString(value: PlistValue | undefined, fallback = ""): string {
  return typeof value === "string" ? value : fallback;
}

function asBoolean(value: PlistValue | undefined, fallback = false): boolean {
  return typeof value === "boolean" ? value : fallback;
}

function readProperties(nodeDict: PlistDict): CcbProperty[] {
  return asArray(nodeDict.properties).map((value) => {
    const dict = asDict(value, "node property");
    return {
      name: asString(dict.name),
      type: asString(dict.type),
      value: dict.value ?? null,
      baseValue: dict.baseValue,
    };
  });
}

function walkNodes(rootNode: PlistDict): CcbNode[] {
  const nodes: CcbNode[] = [];
  let nextId = 0;

  function walk(nodeDict: PlistDict, parentId: number | null, depth: number, path: string): CcbNode {
    const properties = readProperties(nodeDict);
    const propertyMap = new Map(properties.map((property) => [property.name, property]));
    const node: CcbNode = {
      id: nextId++,
      path,
      depth,
      parentId,
      baseClass: asString(nodeDict.baseClass, "Unknown"),
      displayName: asString(nodeDict.displayName, ""),
      properties,
      propertyMap,
      animatedProperties: asDict(nodeDict.animatedProperties ?? {}, "animatedProperties"),
      children: [],
    };

    nodes.push(node);
    node.children = asArray(nodeDict.children).map((child, childIndex) =>
      walk(asDict(child, "child node"), node.id, depth + 1, `${path}.${childIndex}`),
    );
    return node;
  }

  walk(rootNode, null, 0, "0");
  return nodes;
}

function readSequences(root: PlistDict): CcbSequence[] {
  return asArray(root.sequences).map((value) => {
    const sequence = asDict(value, "sequence");
    return {
      sequenceId: asNumber(sequence.sequenceId),
      name: asString(sequence.name),
      length: asNumber(sequence.length),
      resolution: asNumber(sequence.resolution),
      autoPlay: asBoolean(sequence.autoPlay),
      chainedSequenceId: asNumber(sequence.chainedSequenceId, -1),
      offset: asNumber(sequence.offset),
      position: asNumber(sequence.position),
      scale: asNumber(sequence.scale),
    };
  });
}

function analyzeCcb(ccbPath: string): AnalyzerResult {
  const xml = readFileSync(ccbPath, "utf8");
  const parsed = parsePlist(xml);
  const root = asDict(parsed, "plist root");
  const nodeGraph = asDict(root.nodeGraph, "nodeGraph");
  const sequences = readSequences(root);
  const nodes = walkNodes(nodeGraph);
  return { root, nodes, sequences };
}

function compact(value: PlistValue | undefined): string {
  if (value === undefined || value === null) {
    return "";
  }
  if (Array.isArray(value)) {
    return `[${value.map(compact).join(", ")}]`;
  }
  if (typeof value === "object") {
    return `{${Object.entries(value)
      .map(([key, entry]) => `${key}: ${compact(entry)}`)
      .join(", ")}}`;
  }
  return String(value);
}

function csvCell(value: unknown): string {
  const text = value === undefined || value === null ? "" : String(value);
  if (/[",\r\n]/.test(text)) {
    return `"${text.replace(/"/g, '""')}"`;
  }
  return text;
}

function writeCsv(path: string, headers: string[], rows: unknown[][]): void {
  const lines = [headers, ...rows].map((row) => row.map(csvCell).join(","));
  writeFileSync(path, `${lines.join("\n")}\n`, "utf8");
}

function propertyValue(node: CcbNode, name: string): PlistValue | undefined {
  return node.propertyMap.get(name)?.value;
}

function propertyBase(node: CcbNode, name: string): PlistValue | undefined {
  return node.propertyMap.get(name)?.baseValue;
}

type SpriteFrameRef = {
  atlasPlist: string;
  frameName: string;
};

function spriteFrameRef(value: PlistValue | undefined): SpriteFrameRef {
  const entries = asArray(value).map((entry) => asString(entry));
  return {
    atlasPlist: entries.find((entry) => entry.endsWith(".plist")) ?? entries[0] ?? "",
    frameName: entries.find((entry) => entry.endsWith(".png")) ?? entries[1] ?? "",
  };
}

function sequenceNameById(sequences: CcbSequence[]): Map<string, string> {
  return new Map(sequences.map((sequence) => [String(sequence.sequenceId), sequence.name]));
}

function animationTrackRows(result: AnalyzerResult): unknown[][] {
  const names = sequenceNameById(result.sequences);
  const rows: unknown[][] = [];

  for (const node of result.nodes) {
    for (const [sequenceId, sequenceValue] of Object.entries(node.animatedProperties)) {
      const sequenceTracks = asDict(sequenceValue, "sequence animation tracks");
      for (const [propertyName, trackValue] of Object.entries(sequenceTracks)) {
        const track = asDict(trackValue, "animation track");
        const keyframes = asArray(track.keyframes).map((keyframe) => asDict(keyframe, "keyframe"));
        rows.push([
          node.id,
          node.path,
          node.parentId ?? "",
          node.depth,
          node.baseClass,
          node.displayName,
          sequenceId,
          names.get(sequenceId) ?? "",
          propertyName,
          asNumber(track.type),
          keyframes.length,
          keyframes.map((keyframe) => compact(keyframe.time)).join(" | "),
          keyframes.map((keyframe) => compact(keyframe.value)).join(" | "),
          keyframes.map((keyframe) => compact(asDict(keyframe.easing, "keyframe easing").type)).join(" | "),
          compact(propertyBase(node, propertyName)),
        ]);
      }
    }
  }

  return rows;
}

function animatedSequenceList(node: CcbNode, propertyName: string, sequences: CcbSequence[]): string {
  const names = sequenceNameById(sequences);
  const values: string[] = [];
  for (const [sequenceId, sequenceValue] of Object.entries(node.animatedProperties)) {
    const tracks = asDict(sequenceValue, "sequence animation tracks");
    if (tracks[propertyName]) {
      values.push(`${sequenceId}:${names.get(sequenceId) ?? ""}`);
    }
  }
  return values.join(" | ");
}

function spriteRows(result: AnalyzerResult): unknown[][] {
  return result.nodes
    .filter((node) => node.baseClass === "CCSprite" || node.propertyMap.has("displayFrame"))
    .map((node) => {
      const displayFrame = spriteFrameRef(propertyValue(node, "displayFrame"));
      const flip = asArray(propertyValue(node, "flip"));
      return [
        node.id,
        node.path,
        node.parentId ?? "",
        node.depth,
        node.displayName,
        displayFrame.atlasPlist,
        displayFrame.frameName,
        compact(propertyValue(node, "position")),
        compact(propertyBase(node, "position")),
        compact(propertyValue(node, "anchorPoint")),
        compact(propertyValue(node, "scale")),
        compact(propertyValue(node, "rotation")),
        compact(propertyBase(node, "rotation")),
        flip[0] ?? false,
        flip[1] ?? false,
        compact(propertyValue(node, "visible")),
        compact(propertyBase(node, "visible")),
        animatedSequenceList(node, "visible", result.sequences),
        animatedSequenceList(node, "position", result.sequences),
        animatedSequenceList(node, "rotation", result.sequences),
        animatedSequenceList(node, "displayFrame", result.sequences),
      ];
    });
}

function nodeLabel(node: CcbNode): string {
  const name = node.displayName || node.baseClass;
  const frame = spriteFrameRef(propertyValue(node, "displayFrame")).frameName;
  const position = compact(propertyValue(node, "position"));
  const visible = compact(propertyValue(node, "visible"));
  const rotation = compact(propertyValue(node, "rotation"));
  const bits = [
    `#${node.id}`,
    node.baseClass,
    name,
    frame ? `frame=${frame}` : "",
    position ? `pos=${position}` : "",
    rotation ? `rot=${rotation}` : "",
    visible ? `visible=${visible}` : "",
  ].filter(Boolean);
  return bits.join(" | ");
}

function renderNodeTree(result: AnalyzerResult): string {
  const lines: string[] = [];
  lines.push("# CCB Node Tree");
  lines.push("");
  lines.push(`- Source: ${inputPath}`);
  lines.push(`- fileType: ${compact(result.root.fileType)}`);
  lines.push(`- fileVersion: ${compact(result.root.fileVersion)}`);
  lines.push(`- nodeCount: ${result.nodes.length}`);
  lines.push(`- spriteCount: ${result.nodes.filter((node) => node.baseClass === "CCSprite").length}`);
  lines.push(`- sequenceCount: ${result.sequences.length}`);
  lines.push("");
  lines.push("## Tree");
  lines.push("");

  for (const node of result.nodes) {
    lines.push(`${"  ".repeat(node.depth)}- ${nodeLabel(node)}`);
    const animated = Object.entries(node.animatedProperties)
      .flatMap(([sequenceId, sequenceValue]) => {
        const tracks = Object.keys(asDict(sequenceValue, "sequence animation tracks")).join(", ");
        if (!tracks) {
          return [];
        }
        const sequence = result.sequences.find((entry) => String(entry.sequenceId) === sequenceId);
        return [`${sequenceId}:${sequence?.name ?? ""}(${tracks})`];
      })
      .join(" / ");
    if (animated) {
      lines.push(`${"  ".repeat(node.depth + 1)}animations: ${animated}`);
    }
  }

  lines.push("");
  return lines.join("\n");
}

function writeOutputs(result: AnalyzerResult, outDir: string): void {
  mkdirSync(outDir, { recursive: true });

  writeFileSync(resolve(outDir, "node_tree.md"), renderNodeTree(result), "utf8");

  writeCsv(
    resolve(outDir, "sequences.csv"),
    ["sequenceId", "name", "length", "resolution", "autoPlay", "chainedSequenceId", "offset", "position", "scale"],
    result.sequences.map((sequence) => [
      sequence.sequenceId,
      sequence.name,
      sequence.length,
      sequence.resolution,
      sequence.autoPlay,
      sequence.chainedSequenceId,
      sequence.offset,
      sequence.position,
      sequence.scale,
    ]),
  );

  writeCsv(
    resolve(outDir, "animations.csv"),
    [
      "nodeId",
      "path",
      "parentId",
      "depth",
      "baseClass",
      "displayName",
      "sequenceId",
      "sequenceName",
      "property",
      "keyframeType",
      "keyframeCount",
      "times",
      "values",
      "easingTypes",
      "baseValue",
    ],
    animationTrackRows(result),
  );

  writeCsv(
    resolve(outDir, "sprites.csv"),
    [
      "nodeId",
      "path",
      "parentId",
      "depth",
      "displayName",
      "atlasPlist",
      "frameName",
      "position",
      "positionBase",
      "anchorPoint",
      "scale",
      "rotation",
      "rotationBase",
      "flipX",
      "flipY",
      "visible",
      "visibleBase",
      "visibleSequences",
      "positionSequences",
      "rotationSequences",
      "displayFrameSequences",
    ],
    spriteRows(result),
  );
}

function main(): void {
  const ccbPath = resolve(inputPath);
  const outDir = resolve(outputDir);
  const result = analyzeCcb(ccbPath);
  writeOutputs(result, outDir);

  const spriteCount = result.nodes.filter((node) => node.baseClass === "CCSprite").length;
  console.log(`Analyzed ${ccbPath}`);
  console.log(`Wrote ${outDir}`);
  console.log(`nodes=${result.nodes.length}, sprites=${spriteCount}, sequences=${result.sequences.length}`);
}

try {
  main();
} catch (error) {
  const message = error instanceof Error ? error.message : String(error);
  console.error(`ccb-analyzer failed: ${message}`);
  process.exitCode = 1;
}
