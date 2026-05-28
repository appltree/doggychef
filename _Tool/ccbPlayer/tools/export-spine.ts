import {
  existsSync,
  mkdirSync,
  readdirSync,
  readFileSync,
  rmSync,
  writeFileSync,
} from "node:fs";
import { basename, dirname, extname, join, relative, resolve } from "node:path";
import { deflateSync, inflateSync } from "node:zlib";
import {
  atlasFrameKey,
  parseAtlasPlist,
  type AtlasFrame,
} from "../src/ccb/atlas.js";
import {
  propertyValue,
  sampleNodePose,
  spriteFrameRef,
  type CcbDocument,
  type CcbNode,
  type CcbSequence,
  type NodePose,
  parseCcb,
} from "../src/ccb/model.js";

type AssetTarget = {
  key: string;
  label: string;
  dataDir: string;
  ccbPath: string;
  atlases: AtlasInput[];
};

type AtlasInput = {
  plistRef: string;
  plistPath: string;
  imagePath: string;
};

type LoadedAtlas = AtlasInput & {
  frames: Map<string, AtlasFrame>;
};

type RgbaImage = {
  width: number;
  height: number;
  data: Uint8Array;
};

type Matrix = {
  a: number;
  b: number;
  c: number;
  d: number;
  e: number;
  f: number;
};

type Bounds = {
  minX: number;
  minY: number;
  maxX: number;
  maxY: number;
};

type SpineJson = {
  skeleton: Record<string, unknown>;
  bones: SpineBone[];
  slots: SpineSlot[];
  skins: SpineSkin[];
  animations: Record<string, SpineAnimation>;
};

type SpineBone = {
  name: string;
  parent?: string;
  x?: number;
  y?: number;
  rotation?: number;
  scaleX?: number;
  scaleY?: number;
};

type SpineSlot = {
  name: string;
  bone: string;
  attachment?: string;
};

type SpineSkin = {
  name: string;
  attachments: Record<string, Record<string, SpineRegionAttachment>>;
};

type SpineRegionAttachment = {
  path: string;
  x?: number;
  y?: number;
  scaleX?: number;
  scaleY?: number;
  rotation?: number;
  width: number;
  height: number;
};

type SpineAnimation = {
  bones?: Record<string, SpineBoneTimeline>;
  slots?: Record<string, SpineSlotTimeline>;
};

type SpineBoneTimeline = {
  translate?: Array<{ time: number; x: number; y: number }>;
  rotate?: Array<{ time: number; value: number }>;
  scale?: Array<{ time: number; x: number; y: number }>;
};

type SpineSlotTimeline = {
  attachment?: Array<{ time: number; name: string | null }>;
};

const outputRoot = resolve("spine");
const requestedKeys = process.argv.slice(2).map(normalizeKey);
const identity: Matrix = { a: 1, b: 0, c: 0, d: 1, e: 0, f: 0 };

function normalizeKey(value: string): string {
  return basename(value)
    .replace(/^animal_/, "")
    .toLowerCase();
}

function discoverTargets(): AssetTarget[] {
  const targets: AssetTarget[] = [];
  for (const entry of readdirSync(resolve("data"), { withFileTypes: true })) {
    if (!entry.isDirectory()) {
      continue;
    }
    const dataDir = resolve("data", entry.name);
    const ccbFiles = readdirSync(dataDir)
      .filter((fileName) => fileName.toLowerCase().endsWith(".ccb"))
      .sort();
    if (ccbFiles.length === 0) {
      continue;
    }
    for (const ccbFile of ccbFiles) {
      const ccbBase = basename(ccbFile, extname(ccbFile));
      const key = entry.name.startsWith("animal_")
        ? entry.name.replace(/^animal_/, "")
        : ccbFiles.length === 1
          ? entry.name
          : ccbBase;
      const label = ccbFiles.length === 1 ? entry.name : ccbBase;
      targets.push({
        key: normalizeKey(key),
        label,
        dataDir,
        ccbPath: join(dataDir, ccbFile),
        atlases: discoverAtlases(dataDir, entry.name),
      });
    }
  }
  return targets.sort((left, right) => left.label.localeCompare(right.label));
}

function discoverAtlases(dataDir: string, dirName: string): AtlasInput[] {
  const plistFiles = readdirSync(dataDir)
    .filter((fileName) => fileName.toLowerCase().endsWith(".plist"))
    .sort();
  return plistFiles.flatMap((plistFile) => {
    const imageFile = `${basename(plistFile, ".plist")}.png`;
    const imagePath = join(dataDir, imageFile);
    if (!existsSync(imagePath)) {
      return [];
    }
    return [
      {
        plistRef: `${dirName.startsWith("staff") ? "staff" : dirName.startsWith("effect") ? "effect" : "animal"}/${plistFile}`,
        plistPath: join(dataDir, plistFile),
        imagePath,
      },
    ];
  });
}

function loadTargets(): AssetTarget[] {
  const targets = discoverTargets();
  if (requestedKeys.length === 0) {
    return targets;
  }
  const selected: AssetTarget[] = [];
  for (const key of requestedKeys) {
    const matches = targets.filter(
      (target) => target.key === key || normalizeKey(target.label) === key,
    );
    if (matches.length === 0) {
      throw new Error(
        `Unknown target '${key}'. Available: ${targets.map((target) => target.key).join(", ")}`,
      );
    }
    selected.push(...matches);
  }
  return selected;
}

function loadAtlases(target: AssetTarget): LoadedAtlas[] {
  return target.atlases.map((atlas) => {
    return {
      ...atlas,
      frames: parseAtlasPlist(
        readFileSync(atlas.plistPath, "utf8"),
        atlas.plistRef,
      ),
    };
  });
}

function crc32(buffer: Uint8Array): number {
  let crc = 0xffffffff;
  for (const byte of buffer) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit++) {
      crc = (crc >>> 1) ^ (crc & 1 ? 0xedb88320 : 0);
    }
  }
  return (crc ^ 0xffffffff) >>> 0;
}

function filterValue(
  filter: number,
  left: number,
  up: number,
  upLeft: number,
): number {
  if (filter === 0) return 0;
  if (filter === 1) return left;
  if (filter === 2) return up;
  if (filter === 3) return Math.floor((left + up) / 2);
  if (filter === 4) {
    const p = left + up - upLeft;
    const pa = Math.abs(p - left);
    const pb = Math.abs(p - up);
    const pc = Math.abs(p - upLeft);
    return pa <= pb && pa <= pc ? left : pb <= pc ? up : upLeft;
  }
  throw new Error(`Unsupported PNG filter ${filter}`);
}

function readPng(path: string): RgbaImage {
  const buffer = readFileSync(path);
  if (buffer.subarray(0, 8).toString("hex") !== "89504e470d0a1a0a") {
    throw new Error(`${path} is not a PNG file`);
  }

  const width = buffer.readUInt32BE(16);
  const height = buffer.readUInt32BE(20);
  const bitDepth = buffer[24];
  const colorType = buffer[25];
  const interlace = buffer[28];
  if (bitDepth !== 8 || interlace !== 0 || ![2, 6].includes(colorType)) {
    throw new Error(
      `${path}: only non-interlaced 8-bit RGB/RGBA PNG files are supported`,
    );
  }

  const bytesPerPixel = colorType === 6 ? 4 : 3;
  const idatParts: Buffer[] = [];
  let offset = 8;
  while (offset < buffer.length) {
    const length = buffer.readUInt32BE(offset);
    const type = buffer.subarray(offset + 4, offset + 8).toString("ascii");
    const data = buffer.subarray(offset + 8, offset + 8 + length);
    if (type === "IDAT") {
      idatParts.push(data);
    }
    offset += 12 + length;
  }

  const raw = inflateSync(Buffer.concat(idatParts));
  const sourceStride = width * bytesPerPixel;
  const sourceData = new Uint8Array(sourceStride * height);
  let rawOffset = 0;
  for (let y = 0; y < height; y++) {
    const filter = raw[rawOffset++];
    const rowStart = y * sourceStride;
    for (let x = 0; x < sourceStride; x++) {
      const value = raw[rawOffset++];
      const left =
        x >= bytesPerPixel ? sourceData[rowStart + x - bytesPerPixel] : 0;
      const up = y > 0 ? sourceData[rowStart + x - sourceStride] : 0;
      const upLeft =
        y > 0 && x >= bytesPerPixel
          ? sourceData[rowStart + x - sourceStride - bytesPerPixel]
          : 0;
      sourceData[rowStart + x] =
        (value + filterValue(filter, left, up, upLeft)) & 0xff;
    }
  }

  const data = new Uint8Array(width * height * 4);
  for (let pixel = 0; pixel < width * height; pixel++) {
    const sourceIndex = pixel * bytesPerPixel;
    const targetIndex = pixel * 4;
    data[targetIndex] = sourceData[sourceIndex];
    data[targetIndex + 1] = sourceData[sourceIndex + 1];
    data[targetIndex + 2] = sourceData[sourceIndex + 2];
    data[targetIndex + 3] = colorType === 6 ? sourceData[sourceIndex + 3] : 255;
  }

  return { width, height, data };
}

function pngChunk(type: string, data: Buffer): Buffer {
  const typeBuffer = Buffer.from(type, "ascii");
  const length = Buffer.alloc(4);
  length.writeUInt32BE(data.length, 0);
  const crc = Buffer.alloc(4);
  crc.writeUInt32BE(crc32(Buffer.concat([typeBuffer, data])), 0);
  return Buffer.concat([length, typeBuffer, data, crc]);
}

function writePng(path: string, image: RgbaImage): void {
  const stride = image.width * 4;
  const raw = Buffer.alloc((stride + 1) * image.height);
  for (let y = 0; y < image.height; y++) {
    raw[y * (stride + 1)] = 0;
    raw.set(
      image.data.subarray(y * stride, y * stride + stride),
      y * (stride + 1) + 1,
    );
  }

  const signature = Buffer.from("89504e470d0a1a0a", "hex");
  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(image.width, 0);
  ihdr.writeUInt32BE(image.height, 4);
  ihdr[8] = 8;
  ihdr[9] = 6;
  mkdirSync(dirname(path), { recursive: true });
  writeFileSync(
    path,
    Buffer.concat([
      signature,
      pngChunk("IHDR", ihdr),
      pngChunk("IDAT", deflateSync(raw)),
      pngChunk("IEND", Buffer.alloc(0)),
    ]),
  );
}

function getPixel(
  image: RgbaImage,
  x: number,
  y: number,
): [number, number, number, number] {
  const index = (y * image.width + x) * 4;
  return [
    image.data[index],
    image.data[index + 1],
    image.data[index + 2],
    image.data[index + 3],
  ];
}

function setPixel(
  image: RgbaImage,
  x: number,
  y: number,
  color: [number, number, number, number],
): void {
  if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
    return;
  }
  const index = (y * image.width + x) * 4;
  image.data[index] = color[0];
  image.data[index + 1] = color[1];
  image.data[index + 2] = color[2];
  image.data[index + 3] = color[3];
}

function cropFrame(atlas: RgbaImage, frame: AtlasFrame): RgbaImage {
  const outputWidth = Math.max(1, Math.round(frame.width));
  const outputHeight = Math.max(1, Math.round(frame.height));
  const cropped: RgbaImage = {
    width: outputWidth,
    height: outputHeight,
    data: new Uint8Array(outputWidth * outputHeight * 4),
  };

  const textureWidth = Math.round(frame.rotated ? frame.height : frame.width);
  const textureHeight = Math.round(frame.rotated ? frame.width : frame.height);
  const sourceX = Math.round(frame.x);
  const sourceY = Math.round(frame.y);

  for (let y = 0; y < textureHeight; y++) {
    for (let x = 0; x < textureWidth; x++) {
      const color = getPixel(atlas, sourceX + x, sourceY + y);
      if (frame.rotated) {
        setPixel(cropped, y, outputHeight - 1 - x, color);
      } else {
        setPixel(cropped, x, y, color);
      }
    }
  }

  return cropped;
}

function findFrame(
  atlases: LoadedAtlas[],
  pose: NodePose,
): AtlasFrame | undefined {
  const frameName = pose.displayFrame?.frameName;
  if (!frameName) {
    return undefined;
  }
  const plistRef = pose.displayFrame?.atlasPlist ?? "";
  for (const atlas of atlases) {
    const direct = atlas.frames.get(atlasFrameKey(plistRef, frameName));
    if (direct) {
      return direct;
    }
  }
  for (const atlas of atlases) {
    const fallback = atlas.frames.get(frameName);
    if (fallback) {
      return fallback;
    }
  }
  return undefined;
}

function uniqueName(base: string, used: Set<string>): string {
  const clean = (base || "node").replace(/[^\w.-]+/g, "_");
  let name = clean;
  let index = 1;
  while (used.has(name)) {
    name = `${clean}_${index++}`;
  }
  used.add(name);
  return name;
}

function stripExtension(value: string): string {
  return value.replace(/\.[^.]+$/i, "");
}

function toSnakeCase(value: string): string {
  return value
    .replace(/([a-z0-9])([A-Z])/g, "$1_$2")
    .replace(/([A-Z]+)([A-Z][a-z])/g, "$1_$2")
    .replace(/[^\w.-]+/g, "_")
    .replace(/_+/g, "_")
    .replace(/^_+|_+$/g, "")
    .toLowerCase();
}

function friendlyPartName(value: string, target: AssetTarget): string {
  let name = stripExtension(value)
    .replace(/^CCNode$/i, "root")
    .replace(/^CCLayer$/i, "layer")
    .replace(/^CCSprite/i, "")
    .replace(new RegExp(`^${target.label}_`, "i"), "")
    .replace(new RegExp(`^${target.key}_`, "i"), "");

  name = toSnakeCase(name)
    .split("_")
    .map((part) => {
      const tongueMatch = /^tung(\d*)$/.exec(part);
      if (tongueMatch)
        return tongueMatch[1] ? `tongue_${tongueMatch[1]}` : "tongue";
      if (part === "anrgry") return "angry";
      if (part === "mouse") return "mouth";
      if (part === "tung") return "tongue";
      return part;
    })
    .join("_");

  return name || "node";
}

function nodeBaseName(target: AssetTarget, node: CcbNode): string {
  if (node.displayName) {
    return friendlyPartName(node.displayName, target);
  }

  if (node.baseClass === "CCSprite") {
    const frameName = spriteFrameRef(
      propertyValue(node, "displayFrame"),
    )?.frameName;
    if (frameName) {
      return friendlyPartName(frameName, target);
    }
  }

  return friendlyPartName(node.baseClass || `node_${node.id}`, target);
}

function round(value: number): number {
  return Number(value.toFixed(4));
}

function poseScale(pose: NodePose): { x: number; y: number } {
  return {
    x: pose.scale.x,
    y: pose.scale.y,
  };
}

function isOffstagePose(pose: NodePose): boolean {
  return Math.abs(pose.position.x) > 250 || Math.abs(pose.position.y) > 250;
}

function multiply(left: Matrix, right: Matrix): Matrix {
  return {
    a: left.a * right.a + left.c * right.b,
    b: left.b * right.a + left.d * right.b,
    c: left.a * right.c + left.c * right.d,
    d: left.b * right.c + left.d * right.d,
    e: left.a * right.e + left.c * right.f + left.e,
    f: left.b * right.e + left.d * right.f + left.f,
  };
}

function normalizedAnchor(pose: NodePose): { x: number; y: number } {
  return {
    x:
      pose.anchorPoint.x >= 0 && pose.anchorPoint.x <= 1
        ? pose.anchorPoint.x
        : 0.5,
    y:
      pose.anchorPoint.y >= 0 && pose.anchorPoint.y <= 1
        ? pose.anchorPoint.y
        : 0.5,
  };
}

function nodeMatrix(
  pose: NodePose,
  contentSize: { width: number; height: number },
): Matrix {
  const radians = (-pose.rotation * Math.PI) / 180;
  const cos = Math.cos(radians);
  const sin = Math.sin(radians);
  const transformed = multiply(
    { a: 1, b: 0, c: 0, d: 1, e: pose.position.x, f: pose.position.y },
    {
      a: cos * pose.scale.x,
      b: sin * pose.scale.x,
      c: -sin * pose.scale.y,
      d: cos * pose.scale.y,
      e: 0,
      f: 0,
    },
  );
  if (
    pose.ignoreAnchorPointForPosition ||
    (contentSize.width === 0 && contentSize.height === 0)
  ) {
    return transformed;
  }
  const anchor = normalizedAnchor(pose);
  return multiply(transformed, {
    a: 1,
    b: 0,
    c: 0,
    d: 1,
    e: -anchor.x * contentSize.width,
    f: -anchor.y * contentSize.height,
  });
}

function transformPoint(
  matrix: Matrix,
  x: number,
  y: number,
): { x: number; y: number } {
  return {
    x: matrix.a * x + matrix.c * y + matrix.e,
    y: matrix.b * x + matrix.d * y + matrix.f,
  };
}

function expandBounds(
  bounds: Bounds | undefined,
  points: Array<{ x: number; y: number }>,
): Bounds {
  const result = bounds ?? {
    minX: Number.POSITIVE_INFINITY,
    minY: Number.POSITIVE_INFINITY,
    maxX: Number.NEGATIVE_INFINITY,
    maxY: Number.NEGATIVE_INFINITY,
  };
  for (const point of points) {
    result.minX = Math.min(result.minX, point.x);
    result.minY = Math.min(result.minY, point.y);
    result.maxX = Math.max(result.maxX, point.x);
    result.maxY = Math.max(result.maxY, point.y);
  }
  return result;
}

function contentSizeForPose(
  atlases: LoadedAtlas[],
  pose: NodePose,
): { width: number; height: number } {
  const frame = findFrame(atlases, pose);
  return frame
    ? {
        width: frame.sourceWidth || frame.width,
        height: frame.sourceHeight || frame.height,
      }
    : { width: 0, height: 0 };
}

function drawnContentSizeForPose(
  atlases: LoadedAtlas[],
  pose: NodePose,
): { width: number; height: number } {
  const frame = findFrame(atlases, pose);
  return frame
    ? {
        width: frame.width,
        height: frame.height,
      }
    : { width: 0, height: 0 };
}

function contentOffsetForPose(
  pose: NodePose,
  contentSize: { width: number; height: number },
): { x: number; y: number } {
  if (
    pose.ignoreAnchorPointForPosition ||
    (contentSize.width === 0 && contentSize.height === 0)
  ) {
    return { x: 0, y: 0 };
  }
  return {
    x: -pose.anchorPoint.x * contentSize.width,
    y: -pose.anchorPoint.y * contentSize.height,
  };
}

function boneFromPose(
  name: string,
  parent: string | undefined,
  pose: NodePose,
): SpineBone {
  const scale = poseScale(pose);
  return compactObject({
    name,
    parent,
    x: round(pose.position.x),
    y: round(pose.position.y),
    rotation: round(-pose.rotation),
    scaleX: round(scale.x),
    scaleY: round(scale.y),
  });
}

function contentBoneFromPose(
  name: string,
  parent: string,
  pose: NodePose,
  contentSize: { width: number; height: number },
): SpineBone {
  const offset = contentOffsetForPose(pose, contentSize);
  return compactObject({
    name,
    parent,
    x: round(offset.x),
    y: round(offset.y),
  });
}

function compactObject<T extends Record<string, unknown>>(value: T): T {
  for (const key of Object.keys(value)) {
    const entry = value[key];
    if (
      entry === undefined ||
      entry === "" ||
      entry === 0 ||
      entry === 1 ||
      (Array.isArray(entry) && entry.length === 0)
    ) {
      delete value[key];
    }
  }
  return value;
}

function poseSequenceId(
  document: CcbDocument,
  node: CcbNode,
  sequence: CcbSequence,
): number {
  const walkin = walkinFallbackSequence(document, sequence);
  return walkin && node.depth >= 2 ? walkin.sequenceId : sequence.sequenceId;
}

function walkinFallbackSequence(
  document: CcbDocument,
  sequence: CcbSequence,
): CcbSequence | undefined {
  if (sequence.name !== "walkout") {
    return undefined;
  }
  const hasSpriteTracks = document.nodes.some((node) => {
    const tracks = node.animatedProperties.get(sequence.sequenceId);
    return node.depth >= 2 && tracks && tracks.size > 0;
  });
  return hasSpriteTracks
    ? undefined
    : document.sequences.find((entry) => entry.name === "walkin");
}

function isSyntheticMirroredWalkout(
  document: CcbDocument,
  sequence: CcbSequence,
): boolean {
  return Boolean(walkinFallbackSequence(document, sequence));
}

function collectSequenceTimes(
  document: CcbDocument,
  sequence: CcbSequence,
): number[] {
  const times = new Set<number>([0, sequence.length]);
  for (const node of document.nodes) {
    const sequenceId = poseSequenceId(document, node, sequence);
    const tracks = node.animatedProperties.get(sequenceId);
    if (!tracks) {
      continue;
    }
    for (const track of tracks.values()) {
      for (const keyframe of track.keyframes) {
        if (keyframe.time >= 0 && keyframe.time <= sequence.length) {
          times.add(keyframe.time);
        }
      }
    }
  }
  return [...times].sort((left, right) => left - right);
}

function isEffectivelyVisible(
  document: CcbDocument,
  node: CcbNode,
  sequence: CcbSequence,
  time: number,
): boolean {
  let current: CcbNode | undefined = node;
  while (current) {
    const pose = sampleNodePose(
      current,
      poseSequenceId(document, current, sequence),
      time,
    );
    if (!pose.visible || isOffstagePose(pose)) {
      return false;
    }
    current =
      current.parentId === null ? undefined : document.nodes[current.parentId];
  }
  return true;
}

function attachmentName(
  document: CcbDocument,
  node: CcbNode,
  sequence: CcbSequence,
  time: number,
  pose: NodePose,
): string | undefined {
  return isEffectivelyVisible(document, node, sequence, time)
    ? pose.displayFrame?.frameName
    : undefined;
}

function buildSpine(
  target: AssetTarget,
  document: CcbDocument,
  atlases: LoadedAtlas[],
): SpineJson {
  const usedNames = new Set<string>();
  const boneNames = new Map<number, string>();
  const contentBoneNames = new Map<number, string>();
  const baseNames = new Map<number, string>();
  const slotNodes = document.nodes.filter(
    (node) =>
      node.baseClass === "CCSprite" || propertyValue(node, "displayFrame"),
  );
  const slotNames = new Map<number, string>();
  const setupSequence = document.sequences[0];
  const mirrorBoneName = document.sequences.some((sequence) =>
    isSyntheticMirroredWalkout(document, sequence),
  )
    ? uniqueName("__mirror", usedNames)
    : undefined;

  for (const node of document.nodes) {
    const baseName = nodeBaseName(target, node);
    baseNames.set(node.id, baseName);
    boneNames.set(node.id, uniqueName(baseName, usedNames));
  }

  for (const node of slotNodes) {
    const baseName =
      baseNames.get(node.id) ?? boneNames.get(node.id) ?? `node_${node.id}`;
    slotNames.set(node.id, uniqueName(`${baseName}_slot`, usedNames));
    if (node.children.length > 0) {
      contentBoneNames.set(
        node.id,
        uniqueName(`${baseName}_content`, usedNames),
      );
    }
  }

  const bones: SpineBone[] = mirrorBoneName ? [{ name: mirrorBoneName }] : [];
  for (const node of document.nodes) {
    const pose = sampleNodePose(node, setupSequence?.sequenceId ?? 0, 0);
    const parentBone =
      node.parentId === null
        ? mirrorBoneName
        : (contentBoneNames.get(node.parentId) ?? boneNames.get(node.parentId));
    const boneName = boneNames.get(node.id) ?? `bone_${node.id}`;
    bones.push(
      boneFromPose(
        boneNames.get(node.id) ?? `bone_${node.id}`,
        parentBone,
        pose,
      ),
    );
    const contentBoneName = contentBoneNames.get(node.id);
    if (contentBoneName) {
      bones.push(
        contentBoneFromPose(
          contentBoneName,
          boneName,
          pose,
          contentSizeForPose(atlases, pose),
        ),
      );
    }
  }

  const slots: SpineSlot[] = [];
  const attachments: Record<string, Record<string, SpineRegionAttachment>> = {};
  const setupTime = 0;
  for (const node of slotNodes) {
    const sequence = setupSequence ?? document.sequences[0];
    const pose = sampleNodePose(node, sequence?.sequenceId ?? 0, setupTime);
    const slotName = slotNames.get(node.id) ?? `slot_${node.id}`;
    const initialAttachment = sequence
      ? attachmentName(document, node, sequence, setupTime, pose)
      : undefined;
    slots.push(
      compactObject({
        name: slotName,
        bone: boneNames.get(node.id) ?? `bone_${node.id}`,
        attachment: initialAttachment,
      }),
    );

    const slotAttachments = (attachments[slotName] =
      attachments[slotName] ?? {});
    collectNodeAttachments(document, node, atlases, slotAttachments);
  }

  const animations: Record<string, SpineAnimation> = {};
  for (const sequence of document.sequences) {
    animations[sequence.name || `sequence_${sequence.sequenceId}`] =
      buildAnimation(
        document,
        sequence,
        atlases,
        boneNames,
        contentBoneNames,
        slotNames,
        slotNodes,
        mirrorBoneName,
      );
  }

  const bounds = estimateBounds(document, atlases);
  return {
    skeleton: {
      hash: "ccb-player",
      spine: "4.3.00",
      x: round(bounds.minX),
      y: round(bounds.minY),
      width: round(bounds.maxX - bounds.minX),
      height: round(bounds.maxY - bounds.minY),
      fps: 30,
      images: "./images/",
      name: target.label,
    },
    bones,
    slots,
    skins: [{ name: "default", attachments }],
    animations,
  };
}

function collectNodeAttachments(
  document: CcbDocument,
  node: CcbNode,
  atlases: LoadedAtlas[],
  output: Record<string, SpineRegionAttachment>,
): void {
  for (const sequence of document.sequences) {
    const times = collectSequenceTimes(document, sequence);
    for (const time of times) {
      const pose = sampleNodePose(
        node,
        poseSequenceId(document, node, sequence),
        time,
      );
      const name = pose.displayFrame?.frameName;
      if (!name || output[name]) {
        continue;
      }
      const frame = findFrame(atlases, pose);
      if (!frame) {
        continue;
      }
      const width = frame.sourceWidth || frame.width;
      const height = frame.sourceHeight || frame.height;
      output[name] = compactObject({
        path: name,
        x: round((0.5 - pose.anchorPoint.x) * width),
        y: round((0.5 - pose.anchorPoint.y) * height),
        scaleX: pose.flipX ? -1 : 1,
        scaleY: pose.flipY ? -1 : 1,
        width: round(width),
        height: round(height),
      });
    }
  }
}

function buildAnimation(
  document: CcbDocument,
  sequence: CcbSequence,
  atlases: LoadedAtlas[],
  boneNames: Map<number, string>,
  contentBoneNames: Map<number, string>,
  slotNames: Map<number, string>,
  slotNodes: CcbNode[],
  mirrorBoneName: string | undefined,
): SpineAnimation {
  const times = collectSequenceTimes(document, sequence);
  const bones: Record<string, SpineBoneTimeline> = {};
  const slots: Record<string, SpineSlotTimeline> = {};

  for (const node of document.nodes) {
    const boneName = boneNames.get(node.id);
    if (!boneName) {
      continue;
    }
    const setupPose = sampleNodePose(
      node,
      document.sequences[0]?.sequenceId ?? 0,
      0,
    );
    const setupScale = poseScale(setupPose);
    const frames = times.map((time) => ({
      time,
      pose: sampleNodePose(
        node,
        poseSequenceId(document, node, sequence),
        time,
      ),
    }));
    bones[boneName] = {
      translate: frames.map((frame) => ({
        time: frame.time,
        x: round(frame.pose.position.x - setupPose.position.x),
        y: round(frame.pose.position.y - setupPose.position.y),
      })),
      rotate: frames.map((frame) => ({
        time: frame.time,
        value: round(setupPose.rotation - frame.pose.rotation),
      })),
      scale: frames.map((frame) => {
        const scale = poseScale(frame.pose);
        return {
          time: frame.time,
          x: round(scale.x / (setupScale.x || 1)),
          y: round(scale.y / (setupScale.y || 1)),
        };
      }),
    };

    const contentBoneName = contentBoneNames.get(node.id);
    if (contentBoneName) {
      const setupOffset = contentOffsetForPose(
        setupPose,
        contentSizeForPose(atlases, setupPose),
      );
      bones[contentBoneName] = {
        translate: frames.map((frame) => {
          const offset = contentOffsetForPose(
            frame.pose,
            contentSizeForPose(atlases, frame.pose),
          );
          return {
            time: frame.time,
            x: round(offset.x - setupOffset.x),
            y: round(offset.y - setupOffset.y),
          };
        }),
      };
    }
  }

  for (const node of slotNodes) {
    const slotName = slotNames.get(node.id);
    if (!slotName) {
      continue;
    }
    const attachmentFrames = times.map((time) => {
      const pose = sampleNodePose(
        node,
        poseSequenceId(document, node, sequence),
        time,
      );
      return {
        time,
        name: attachmentName(document, node, sequence, time, pose) ?? null,
      };
    });
    slots[slotName] = {
      attachment: collapseAttachmentFrames(attachmentFrames),
    };
  }

  if (mirrorBoneName && isSyntheticMirroredWalkout(document, sequence)) {
    const bounds = estimateSequenceBounds(document, atlases, sequence);
    const centerX = (bounds.minX + bounds.maxX) / 2;
    bones[mirrorBoneName] = {
      translate: times.map((time) => ({
        time,
        x: round(centerX * 2),
        y: 0,
      })),
      scale: times.map((time) => ({
        time,
        x: -1,
        y: 1,
      })),
    };
  }

  return compactObject({ bones, slots });
}

function collapseAttachmentFrames(
  frames: Array<{ time: number; name: string | null }>,
): Array<{ time: number; name: string | null }> {
  const collapsed: Array<{ time: number; name: string | null }> = [];
  for (const frame of frames) {
    if (
      collapsed.length === 0 ||
      collapsed[collapsed.length - 1].name !== frame.name
    ) {
      collapsed.push(frame);
    }
  }
  return collapsed;
}

function walkSequenceBounds(
  document: CcbDocument,
  atlases: LoadedAtlas[],
  node: CcbNode,
  sequence: CcbSequence,
  time: number,
  parentMatrix: Matrix,
  parentVisible: boolean,
  bounds: Bounds | undefined,
): Bounds | undefined {
  const pose = sampleNodePose(
    node,
    poseSequenceId(document, node, sequence),
    time,
  );
  const visible = parentVisible && pose.visible && !isOffstagePose(pose);
  const contentSize = drawnContentSizeForPose(atlases, pose);
  const matrix = multiply(parentMatrix, nodeMatrix(pose, contentSize));
  if (visible && pose.displayFrame) {
    const frame = findFrame(atlases, pose);
    if (frame) {
      bounds = expandBounds(bounds, [
        transformPoint(matrix, 0, 0),
        transformPoint(matrix, frame.width, 0),
        transformPoint(matrix, 0, frame.height),
        transformPoint(matrix, frame.width, frame.height),
      ]);
    }
  }
  for (const child of node.children) {
    bounds = walkSequenceBounds(
      document,
      atlases,
      child,
      sequence,
      time,
      matrix,
      visible,
      bounds,
    );
  }
  return bounds;
}

function estimateSequenceBounds(
  document: CcbDocument,
  atlases: LoadedAtlas[],
  sequence: CcbSequence,
): Bounds {
  let bounds: Bounds | undefined;
  const steps = Math.max(2, Math.ceil(sequence.length * 12));
  for (let index = 0; index <= steps; index++) {
    const time = (sequence.length * index) / steps;
    bounds = walkSequenceBounds(
      document,
      atlases,
      document.root,
      sequence,
      time,
      identity,
      true,
      bounds,
    );
  }
  return bounds ?? { minX: -120, minY: -120, maxX: 120, maxY: 120 };
}

function estimateBounds(document: CcbDocument, atlases: LoadedAtlas[]): Bounds {
  let minX = Number.POSITIVE_INFINITY;
  let minY = Number.POSITIVE_INFINITY;
  let maxX = Number.NEGATIVE_INFINITY;
  let maxY = Number.NEGATIVE_INFINITY;
  for (const sequence of document.sequences) {
    for (const node of document.nodes) {
      for (const time of collectSequenceTimes(document, sequence)) {
        const pose = sampleNodePose(
          node,
          poseSequenceId(document, node, sequence),
          time,
        );
        const frame = findFrame(atlases, pose);
        if (!pose.visible || isOffstagePose(pose) || !frame) {
          continue;
        }
        minX = Math.min(
          minX,
          pose.position.x - pose.anchorPoint.x * frame.sourceWidth,
        );
        minY = Math.min(
          minY,
          pose.position.y - pose.anchorPoint.y * frame.sourceHeight,
        );
        maxX = Math.max(
          maxX,
          pose.position.x + (1 - pose.anchorPoint.x) * frame.sourceWidth,
        );
        maxY = Math.max(
          maxY,
          pose.position.y + (1 - pose.anchorPoint.y) * frame.sourceHeight,
        );
      }
    }
  }
  if (!Number.isFinite(minX)) {
    return { minX: -120, minY: -120, maxX: 120, maxY: 120 };
  }
  return { minX, minY, maxX, maxY };
}

function writeAttachmentImages(atlases: LoadedAtlas[], outDir: string): void {
  const imageDir = join(outDir, "images");
  mkdirSync(imageDir, { recursive: true });
  const written = new Set<string>();
  for (const atlas of atlases) {
    const page = readPng(atlas.imagePath);
    const frames = [...atlas.frames.values()].filter(
      (frame, index, all) =>
        all.findIndex((entry) => entry.name === frame.name) === index,
    );
    for (const frame of frames) {
      if (written.has(frame.name)) {
        continue;
      }
      written.add(frame.name);
      writePng(join(imageDir, frame.name), cropFrame(page, frame));
    }
  }
}

function cleanOutputDir(target: AssetTarget, outDir: string): void {
  mkdirSync(outDir, { recursive: true });
  const obsoletePaths = [
    join(outDir, `${target.key}.atlas`),
    ...target.atlases.map((atlas) => join(outDir, basename(atlas.imagePath))),
  ];
  for (const obsoletePath of obsoletePaths) {
    if (existsSync(obsoletePath)) {
      rmSync(obsoletePath, { force: true });
    }
  }
}

function exportTarget(target: AssetTarget): void {
  const document = parseCcb(readFileSync(target.ccbPath, "utf8"));
  const atlases = loadAtlases(target);
  const spine = buildSpine(target, document, atlases);
  const outputName =
    target.label ||
    relative(resolve("data"), dirname(target.ccbPath)).replace(/[\\/]/g, "_");
  const outDir = join(outputRoot, outputName);
  cleanOutputDir(target, outDir);
  writeFileSync(
    join(outDir, `${target.key}.json`),
    `${JSON.stringify(spine, null, 2)}\n`,
    "utf8",
  );
  writeAttachmentImages(atlases, outDir);
  console.log(`Exported ${target.label} -> ${relative(process.cwd(), outDir)}`);
}

function main(): void {
  const targets = loadTargets();
  if (targets.length === 0) {
    throw new Error("No CCB targets found under data/");
  }
  mkdirSync(outputRoot, { recursive: true });
  for (const target of targets) {
    exportTarget(target);
  }
}

try {
  main();
} catch (error) {
  const message = error instanceof Error ? error.message : String(error);
  console.error(`export-spine failed: ${message}`);
  process.exitCode = 1;
}
