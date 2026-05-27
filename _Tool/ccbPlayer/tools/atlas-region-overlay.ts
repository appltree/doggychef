import { existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { dirname, relative, resolve } from "node:path";

type CsvRow = Record<string, string>;

type AtlasFrame = {
  name: string;
  x: number;
  y: number;
  width: number;
  height: number;
  rotated: boolean;
  offset?: Point;
  sourceSize?: Size;
};

type Point = {
  x: number;
  y: number;
};

type Size = {
  width: number;
  height: number;
};

const spritesCsvPath = process.argv[2] ?? "analysis/animal_horse/sprites.csv";
const atlasPlistPath =
  process.argv[3] ?? "data/animal_horse/animal_horse.plist";
const atlasPngPath = process.argv[4] ?? "data/animal_horse/animal_horse.png";
const outputSvgPath =
  process.argv[5] ?? "analysis/animal_horse/animal_horse_atlas_regions.svg";

function decodeXml(text: string): string {
  return text
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'")
    .replace(/&amp;/g, "&");
}

function escapeXml(text: string): string {
  return text
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;");
}

function parseCsv(text: string): CsvRow[] {
  const rows: string[][] = [];
  let row: string[] = [];
  let cell = "";
  let quoted = false;

  for (let index = 0; index < text.length; index++) {
    const char = text[index];
    const next = text[index + 1];

    if (quoted) {
      if (char === '"' && next === '"') {
        cell += '"';
        index++;
      } else if (char === '"') {
        quoted = false;
      } else {
        cell += char;
      }
      continue;
    }

    if (char === '"') {
      quoted = true;
    } else if (char === ",") {
      row.push(cell.trim());
      cell = "";
    } else if (char === "\n") {
      row.push(cell.trim());
      rows.push(row);
      row = [];
      cell = "";
    } else if (char !== "\r") {
      cell += char;
    }
  }

  if (cell || row.length) {
    row.push(cell.trim());
    rows.push(row);
  }

  const [headers, ...dataRows] = rows.filter((entry) =>
    entry.some((cellValue) => cellValue),
  );
  if (!headers) {
    return [];
  }

  return dataRows.map((dataRow) => {
    const result: CsvRow = {};
    headers.forEach((header, index) => {
      result[header.trim()] = dataRow[index]?.trim() ?? "";
    });
    return result;
  });
}

function parseRect(
  value: string,
): { x: number; y: number; width: number; height: number } | undefined {
  const match =
    /\{\{\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\}\s*,\s*\{\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\}\s*\}/.exec(
      value,
    );
  if (!match) {
    return undefined;
  }
  return {
    x: Number.parseFloat(match[1]),
    y: Number.parseFloat(match[2]),
    width: Number.parseFloat(match[3]),
    height: Number.parseFloat(match[4]),
  };
}

function parsePoint(value: string): Point | undefined {
  const match = /\{\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\}/.exec(
    value,
  );
  if (!match) {
    return undefined;
  }
  return {
    x: Number.parseFloat(match[1]),
    y: Number.parseFloat(match[2]),
  };
}

function parseSize(value: string): Size | undefined {
  const point = parsePoint(value);
  if (!point) {
    return undefined;
  }
  return { width: point.x, height: point.y };
}

function readPngSize(path: string): Size {
  const buffer = readFileSync(path);
  const signature = buffer.subarray(0, 8).toString("hex");
  if (signature !== "89504e470d0a1a0a") {
    throw new Error(`${path} is not a PNG file`);
  }
  return {
    width: buffer.readUInt32BE(16),
    height: buffer.readUInt32BE(20),
  };
}

function readKeyString(body: string, key: string): string | undefined {
  const pattern = new RegExp(
    `<key>${key}</key>\\s*<string>([\\s\\S]*?)</string>`,
  );
  const match = pattern.exec(body);
  return match ? decodeXml(match[1]) : undefined;
}

function readKeyBoolean(body: string, key: string): boolean {
  const pattern = new RegExp(`<key>${key}</key>\\s*<(true|false)\\s*/>`);
  const match = pattern.exec(body);
  return match?.[1] === "true";
}

function readAtlasFrames(path: string): Map<string, AtlasFrame> {
  const xml = readFileSync(path, "utf8");
  const frames = new Map<string, AtlasFrame>();
  const entryPattern = /<key>([^<]+\.png)<\/key>\s*<dict>([\s\S]*?)<\/dict>/g;

  for (const match of xml.matchAll(entryPattern)) {
    const name = decodeXml(match[1]);
    const body = match[2];
    const rectValue =
      readKeyString(body, "textureRect") ?? readKeyString(body, "frame");
    const rect = rectValue ? parseRect(rectValue) : undefined;
    if (!rect) {
      continue;
    }

    const rotated =
      readKeyBoolean(body, "textureRotated") || readKeyBoolean(body, "rotated");
    const offsetValue =
      readKeyString(body, "spriteOffset") ?? readKeyString(body, "offset");
    const sourceSizeValue =
      readKeyString(body, "spriteSourceSize") ??
      readKeyString(body, "sourceSize");

    frames.set(name, {
      name,
      x: rect.x,
      y: rect.y,
      width: rect.width,
      height: rect.height,
      rotated,
      offset: offsetValue ? parsePoint(offsetValue) : undefined,
      sourceSize: sourceSizeValue ? parseSize(sourceSizeValue) : undefined,
    });
  }

  return frames;
}

function normalizeFrameName(row: CsvRow): string {
  if (row.frameName.endsWith(".png")) {
    return row.frameName;
  }
  if (row.atlasPlist.endsWith(".png")) {
    return row.atlasPlist;
  }
  return row.frameName;
}

function frameNamesFromCompactSpriteFrames(value: string): string[] {
  const names = new Set<string>();
  const pattern = /\[([^\]]+)\]/g;
  for (const match of value.matchAll(pattern)) {
    const frameName = match[1]
      .split(",")
      .map((entry) => entry.trim())
      .find((entry) => entry.endsWith(".png"));
    if (frameName) {
      names.add(frameName);
    }
  }
  return [...names];
}

function animationRowsForSprites(spritesCsvPath: string): CsvRow[] {
  const animationsCsvPath = resolve(dirname(spritesCsvPath), "animations.csv");
  if (!existsSync(animationsCsvPath)) {
    return [];
  }

  return parseCsv(readFileSync(animationsCsvPath, "utf8"))
    .filter((row) => row.property === "displayFrame")
    .flatMap((row) =>
      frameNamesFromCompactSpriteFrames(`${row.values} | ${row.baseValue}`).map(
        (frameName) => ({
          nodeId: row.nodeId,
          displayName: row.displayName,
          frameName,
        }),
      ),
    );
}

function groupSpritesByFrame(rows: CsvRow[], animationRows: CsvRow[]): Map<string, CsvRow[]> {
  const grouped = new Map<string, CsvRow[]>();
  for (const row of [...rows, ...animationRows]) {
    const frameName = normalizeFrameName(row);
    if (!frameName) {
      continue;
    }
    grouped.set(frameName, [...(grouped.get(frameName) ?? []), row]);
  }
  return grouped;
}

function frameFamilyPrefix(frameName: string): string | undefined {
  const match = /^(.*_(?:eye|mouse)_)\d+\.png$/.exec(frameName);
  return match?.[1];
}

function includeRelatedAtlasFrames(
  grouped: Map<string, CsvRow[]>,
  atlasFrames: Map<string, AtlasFrame>,
): void {
  const prefixes = new Set(
    [...grouped.keys()]
      .map(frameFamilyPrefix)
      .filter((prefix): prefix is string => Boolean(prefix)),
  );

  for (const frameName of atlasFrames.keys()) {
    const prefix = frameFamilyPrefix(frameName);
    if (!prefixes.has(prefix ?? "") || grouped.has(frameName)) {
      continue;
    }
    grouped.set(frameName, [
      {
        nodeId: "*",
        displayName: "related atlas frame",
        frameName,
      },
    ]);
  }
}

function svgImageHref(imagePath: string, outputPath: string): string {
  const outputDir = dirname(resolve(outputPath));
  return relative(outputDir, resolve(imagePath)).replace(/\\/g, "/");
}

function renderOverlaySvg(options: {
  imagePath: string;
  outputPath: string;
  imageSize: Size;
  usedFrames: Map<string, CsvRow[]>;
  atlasFrames: Map<string, AtlasFrame>;
}): { svg: string; missingFrames: string[]; rotatedCount: number } {
  const { imagePath, outputPath, imageSize, usedFrames, atlasFrames } = options;
  const colors = [
    "#ff3c3c",
    "#3cbeff",
    "#50e678",
    "#ffd246",
    "#dc64ff",
    "#ff8228",
  ];
  const missingFrames: string[] = [];
  let rotatedCount = 0;
  const shapes: string[] = [];
  let colorIndex = 0;

  for (const frameName of [...usedFrames.keys()].sort()) {
    const frame = atlasFrames.get(frameName);
    if (!frame) {
      missingFrames.push(frameName);
      continue;
    }

    const rows = usedFrames.get(frameName) ?? [];
    const color = colors[colorIndex % colors.length];
    const ids = rows.map((row) => row.nodeId).join("/");
    const label = `#${ids.length > 18 ? `${ids.slice(0, 18)}+` : ids}${frame.rotated ? " ROT" : ""}`;
    const labelX = Math.max(
      0,
      Math.min(frame.x, imageSize.width - Math.max(42, label.length * 7)),
    );
    const labelY = Math.max(0, frame.y - 14);
    const titleParts = [
      frame.name,
      `nodes=${ids}`,
      `rect=${frame.x},${frame.y},${frame.width},${frame.height}`,
      `rotated=${frame.rotated}`,
      frame.sourceSize
        ? `sourceSize=${frame.sourceSize.width}x${frame.sourceSize.height}`
        : "",
      frame.offset ? `offset=${frame.offset.x},${frame.offset.y}` : "",
    ].filter(Boolean);

    if (frame.rotated) {
      rotatedCount++;
    }

    const textureWidth = frame.rotated ? frame.height : frame.width;
    const textureHeight = frame.rotated ? frame.width : frame.height;

    shapes.push(
      [
        `<g>`,
        `<title>${escapeXml(titleParts.join(" | "))}</title>`,
        `<rect x="${frame.x}" y="${frame.y}" width="${textureWidth}" height="${textureHeight}" fill="none" stroke="${color}" stroke-width="2" ${
          frame.rotated ? 'stroke-dasharray="5 3"' : ""
        }/>`,
        frame.rotated
          ? `<path d="M ${frame.x + textureWidth - 13} ${frame.y + 4} l 8 0 l 0 8 M ${frame.x + textureWidth - 5} ${frame.y + 4} q -10 0 -10 10" fill="none" stroke="${color}" stroke-width="2"/>`
          : "",
        `<rect x="${labelX}" y="${labelY}" width="${Math.max(38, label.length * 7)}" height="14" fill="rgba(0,0,0,0.78)"/>`,
        `<text x="${labelX + 2}" y="${labelY + 10}" fill="${color}" font-family="Consolas, monospace" font-size="10">${escapeXml(label)}</text>`,
        `</g>`,
      ].join("\n"),
    );
    colorIndex++;
  }

  const href = svgImageHref(imagePath, outputPath);
  const summary = `frames=${usedFrames.size} plist=${atlasFrames.size} rotated=${rotatedCount} missing=${missingFrames.length}`;
  const canvasWidth = Math.max(imageSize.width, 340, summary.length * 8);
  const svg = [
    `<?xml version="1.0" encoding="UTF-8"?>`,
    `<svg xmlns="http://www.w3.org/2000/svg" width="${canvasWidth}" height="${imageSize.height}" viewBox="0 0 ${canvasWidth} ${imageSize.height}">`,
    `<rect x="0" y="0" width="${imageSize.width}" height="${imageSize.height}" fill="#ff00ff"/>`,
    `<image href="${escapeXml(href)}" x="0" y="0" width="${imageSize.width}" height="${imageSize.height}"/>`,
    `<rect x="0" y="0" width="${canvasWidth}" height="20" fill="rgba(0,0,0,0.82)"/>`,
    `<text x="4" y="14" fill="#fff" font-family="Consolas, monospace" font-size="12">${escapeXml(summary)}</text>`,
    ...shapes,
    `</svg>`,
    "",
  ].join("\n");

  return { svg, missingFrames, rotatedCount };
}

function main(): void {
  const spritesCsv = resolve(spritesCsvPath);
  const atlasPlist = resolve(atlasPlistPath);
  const atlasPng = resolve(atlasPngPath);
  const outputSvg = resolve(outputSvgPath);

  const rows = parseCsv(readFileSync(spritesCsv, "utf8"));
  const usedFrames = groupSpritesByFrame(rows, animationRowsForSprites(spritesCsv));
  const atlasFrames = readAtlasFrames(atlasPlist);
  includeRelatedAtlasFrames(usedFrames, atlasFrames);
  const imageSize = readPngSize(atlasPng);
  const { svg, missingFrames, rotatedCount } = renderOverlaySvg({
    imagePath: atlasPng,
    outputPath: outputSvg,
    imageSize,
    usedFrames,
    atlasFrames,
  });

  mkdirSync(dirname(outputSvg), { recursive: true });
  writeFileSync(outputSvg, svg, "utf8");

  console.log(`Wrote ${outputSvg}`);
  console.log(
    `usedFrames=${usedFrames.size}, plistFrames=${atlasFrames.size}, rotatedUsedFrames=${rotatedCount}, missing=${missingFrames.length}`,
  );
  if (missingFrames.length) {
    console.log(`missingFrames=${missingFrames.join(", ")}`);
  }
}

try {
  main();
} catch (error) {
  const message = error instanceof Error ? error.message : String(error);
  console.error(`atlas-region-overlay failed: ${message}`);
  process.exitCode = 1;
}
