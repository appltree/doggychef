import { existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { deflateSync, inflateSync } from "node:zlib";

type CsvRow = Record<string, string>;

type AtlasFrame = {
  name: string;
  x: number;
  y: number;
  width: number;
  height: number;
  rotated: boolean;
};

type RgbaImage = {
  width: number;
  height: number;
  data: Uint8Array;
};

const spritesCsvPath = process.argv[2] ?? "analysis/animal_dear/sprites.csv";
const atlasPlistPath = process.argv[3] ?? "data/animal_dear/animal_dear.plist";
const atlasPngPath = process.argv[4] ?? "data/animal_dear/animal_dear.png";
const outputPngPath = process.argv[5] ?? "analysis/animal_dear/animal_dear_frame_preview.png";

const glyphs: Record<string, string[]> = {
  "0": ["111", "101", "101", "101", "111"],
  "1": ["010", "110", "010", "010", "111"],
  "2": ["111", "001", "111", "100", "111"],
  "3": ["111", "001", "111", "001", "111"],
  "4": ["101", "101", "111", "001", "001"],
  "5": ["111", "100", "111", "001", "111"],
  "6": ["111", "100", "111", "101", "111"],
  "7": ["111", "001", "010", "010", "010"],
  "8": ["111", "101", "111", "101", "111"],
  "9": ["111", "101", "111", "001", "111"],
  "#": ["101", "111", "101", "111", "101"],
  "/": ["001", "001", "010", "100", "100"],
  "+": ["000", "010", "111", "010", "000"],
  "C": ["111", "100", "100", "100", "111"],
  "O": ["111", "101", "101", "101", "111"],
  "R": ["110", "101", "110", "101", "101"],
  "T": ["111", "010", "010", "010", "010"],
  "W": ["101", "101", "101", "111", "101"],
  " ": ["000", "000", "000", "000", "000"],
};

function decodeXml(text: string): string {
  return text
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'")
    .replace(/&amp;/g, "&");
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
    } else if (char === '"') {
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

  const [headers, ...dataRows] = rows.filter((entry) => entry.some(Boolean));
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

function parseRect(value: string): Pick<AtlasFrame, "x" | "y" | "width" | "height"> | undefined {
  const match = /\{\{\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\}\s*,\s*\{\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\}\s*\}/.exec(
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

function readKeyString(body: string, key: string): string | undefined {
  const match = new RegExp(`<key>${key}</key>\\s*<string>([\\s\\S]*?)</string>`).exec(body);
  return match ? decodeXml(match[1]) : undefined;
}

function readKeyBoolean(body: string, key: string): boolean {
  return new RegExp(`<key>${key}</key>\\s*<(true|false)\\s*/>`).exec(body)?.[1] === "true";
}

function readAtlasFrames(path: string): Map<string, AtlasFrame> {
  const xml = readFileSync(path, "utf8");
  const frames = new Map<string, AtlasFrame>();
  const entryPattern = /<key>([^<]+\.png)<\/key>\s*<dict>([\s\S]*?)<\/dict>/g;

  for (const match of xml.matchAll(entryPattern)) {
    const name = decodeXml(match[1]);
    const body = match[2];
    const rectValue = readKeyString(body, "textureRect") ?? readKeyString(body, "frame");
    const rect = rectValue ? parseRect(rectValue) : undefined;
    if (!rect) {
      continue;
    }
    frames.set(name, {
      name,
      ...rect,
      rotated: readKeyBoolean(body, "textureRotated") || readKeyBoolean(body, "rotated"),
    });
  }

  return frames;
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
  if (bitDepth !== 8 || colorType !== 6 || interlace !== 0) {
    throw new Error("Only non-interlaced 8-bit RGBA PNG files are supported");
  }

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
  const stride = width * 4;
  const data = new Uint8Array(width * height * 4);
  let rawOffset = 0;
  for (let y = 0; y < height; y++) {
    const filter = raw[rawOffset++];
    const rowStart = y * stride;
    for (let x = 0; x < stride; x++) {
      const value = raw[rawOffset++];
      const left = x >= 4 ? data[rowStart + x - 4] : 0;
      const up = y > 0 ? data[rowStart + x - stride] : 0;
      const upLeft = y > 0 && x >= 4 ? data[rowStart + x - stride - 4] : 0;
      data[rowStart + x] = (value + filterValue(filter, left, up, upLeft)) & 0xff;
    }
  }
  return { width, height, data };
}

function filterValue(filter: number, left: number, up: number, upLeft: number): number {
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

function writePng(path: string, image: RgbaImage): void {
  const stride = image.width * 4;
  const raw = Buffer.alloc((stride + 1) * image.height);
  for (let y = 0; y < image.height; y++) {
    raw[y * (stride + 1)] = 0;
    raw.set(image.data.subarray(y * stride, y * stride + stride), y * (stride + 1) + 1);
  }

  const chunks: Buffer[] = [];
  const signature = Buffer.from("89504e470d0a1a0a", "hex");
  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(image.width, 0);
  ihdr.writeUInt32BE(image.height, 4);
  ihdr[8] = 8;
  ihdr[9] = 6;
  chunks.push(signature, pngChunk("IHDR", ihdr), pngChunk("IDAT", deflateSync(raw)), pngChunk("IEND", Buffer.alloc(0)));
  mkdirSync(dirname(path), { recursive: true });
  writeFileSync(path, Buffer.concat(chunks));
}

function pngChunk(type: string, data: Buffer): Buffer {
  const typeBuffer = Buffer.from(type, "ascii");
  const length = Buffer.alloc(4);
  length.writeUInt32BE(data.length, 0);
  const crc = Buffer.alloc(4);
  crc.writeUInt32BE(crc32(Buffer.concat([typeBuffer, data])), 0);
  return Buffer.concat([length, typeBuffer, data, crc]);
}

function getPixel(image: RgbaImage, x: number, y: number): [number, number, number, number] {
  const index = (y * image.width + x) * 4;
  return [image.data[index], image.data[index + 1], image.data[index + 2], image.data[index + 3]];
}

function setPixel(image: RgbaImage, x: number, y: number, color: [number, number, number, number]): void {
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
  const cropped: RgbaImage = {
    width: frame.width,
    height: frame.height,
    data: new Uint8Array(frame.width * frame.height * 4),
  };

  const textureWidth = frame.rotated ? frame.height : frame.width;
  const textureHeight = frame.rotated ? frame.width : frame.height;

  for (let y = 0; y < textureHeight; y++) {
    for (let x = 0; x < textureWidth; x++) {
      const color = getPixel(atlas, frame.x + x, frame.y + y);
      if (frame.rotated) {
        setPixel(cropped, y, frame.height - 1 - x, color);
      } else {
        setPixel(cropped, x, y, color);
      }
    }
  }

  return cropped;
}

function drawRect(image: RgbaImage, x: number, y: number, width: number, height: number, color: [number, number, number, number]): void {
  for (let px = x; px < x + width; px++) {
    setPixel(image, px, y, color);
    setPixel(image, px, y + height - 1, color);
  }
  for (let py = y; py < y + height; py++) {
    setPixel(image, x, py, color);
    setPixel(image, x + width - 1, py, color);
  }
}

function fillChecker(image: RgbaImage, x: number, y: number, width: number, height: number): void {
  const light: [number, number, number, number] = [236, 236, 236, 255];
  const dark: [number, number, number, number] = [180, 180, 180, 255];
  for (let py = 0; py < height; py++) {
    for (let px = 0; px < width; px++) {
      setPixel(image, x + px, y + py, (Math.floor(px / 4) + Math.floor(py / 4)) % 2 === 0 ? light : dark);
    }
  }
}

function drawText(image: RgbaImage, text: string, x: number, y: number, color: [number, number, number, number]): void {
  let cursor = x;
  for (const char of text) {
    const glyph = glyphs[char] ?? glyphs[" "];
    glyph.forEach((line, gy) => {
      [...line].forEach((pixel, gx) => {
        if (pixel === "1") {
          setPixel(image, cursor + gx, y + gy, color);
        }
      });
    });
    cursor += 4;
  }
}

function compactNodeIds(ids: string[]): string {
  if (ids.length <= 2) {
    return ids.join("/");
  }
  return `${ids.slice(0, 2).join("/")}+${ids.length - 2}`;
}

function blit(target: RgbaImage, source: RgbaImage, x: number, y: number): void {
  for (let sy = 0; sy < source.height; sy++) {
    for (let sx = 0; sx < source.width; sx++) {
      const sourceColor = getPixel(source, sx, sy);
      const alpha = sourceColor[3] / 255;
      if (alpha <= 0) {
        continue;
      }
      if (alpha >= 1) {
        setPixel(target, x + sx, y + sy, sourceColor);
        continue;
      }
      const targetColor = getPixel(target, x + sx, y + sy);
      setPixel(target, x + sx, y + sy, [
        Math.round(sourceColor[0] * alpha + targetColor[0] * (1 - alpha)),
        Math.round(sourceColor[1] * alpha + targetColor[1] * (1 - alpha)),
        Math.round(sourceColor[2] * alpha + targetColor[2] * (1 - alpha)),
        255,
      ]);
    }
  }
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
  frames: Map<string, AtlasFrame>,
): void {
  const prefixes = new Set(
    [...grouped.keys()]
      .map(frameFamilyPrefix)
      .filter((prefix): prefix is string => Boolean(prefix)),
  );

  for (const frameName of frames.keys()) {
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

function makePreview(atlas: RgbaImage, frames: Map<string, AtlasFrame>, usedFrames: Map<string, CsvRow[]>): RgbaImage {
  const cellWidth = 112;
  const cellHeight = 96;
  const columns = 3;
  const entries = [...usedFrames.keys()].sort();
  const rows = Math.ceil(entries.length / columns);
  const output: RgbaImage = {
    width: cellWidth * columns,
    height: cellHeight * rows,
    data: new Uint8Array(cellWidth * columns * cellHeight * rows * 4),
  };

  for (let index = 0; index < output.data.length; index += 4) {
    output.data[index] = 18;
    output.data[index + 1] = 18;
    output.data[index + 2] = 18;
    output.data[index + 3] = 255;
  }

  entries.forEach((frameName, index) => {
    const frame = frames.get(frameName);
    if (!frame) {
      return;
    }
    const col = index % columns;
    const row = Math.floor(index / columns);
    const cellX = col * cellWidth;
    const cellY = row * cellHeight;
    const normalized = cropFrame(atlas, frame);
    const drawX = cellX + Math.floor((cellWidth - normalized.width) / 2);
    const drawY = cellY + 14 + Math.floor((cellHeight - 20 - normalized.height) / 2);
    const color: [number, number, number, number] = frame.rotated ? [255, 80, 80, 255] : [80, 220, 120, 255];
    fillChecker(output, drawX, drawY, normalized.width, normalized.height);
    blit(output, normalized, drawX, drawY);
    drawRect(output, drawX - 1, drawY - 1, normalized.width + 2, normalized.height + 2, color);
    const ids = (usedFrames.get(frameName) ?? []).map((entry) => entry.nodeId);
    drawText(output, `#${compactNodeIds(ids)}${frame.rotated ? " ROT" : ""}`, cellX + 3, cellY + 4, color);
  });

  return output;
}

function main(): void {
  const spritesCsv = resolve(spritesCsvPath);
  const rows = parseCsv(readFileSync(spritesCsv, "utf8"));
  const frames = readAtlasFrames(resolve(atlasPlistPath));
  const atlas = readPng(resolve(atlasPngPath));
  const usedFrames = groupSpritesByFrame(rows, animationRowsForSprites(spritesCsv));
  includeRelatedAtlasFrames(usedFrames, frames);
  const preview = makePreview(atlas, frames, usedFrames);
  const output = resolve(outputPngPath);
  writePng(output, preview);

  const rotatedUsedFrames = [...usedFrames.keys()].filter((frameName) => frames.get(frameName)?.rotated).length;
  console.log(`Wrote ${output}`);
  console.log(`usedFrames=${usedFrames.size}, rotatedUsedFrames=${rotatedUsedFrames}`);
}

try {
  main();
} catch (error) {
  const message = error instanceof Error ? error.message : String(error);
  console.error(`atlas-frame-preview failed: ${message}`);
  process.exitCode = 1;
}
