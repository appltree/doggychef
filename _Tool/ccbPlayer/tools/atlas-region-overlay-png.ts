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

type Rgba = [number, number, number, number];

type RgbaImage = {
  width: number;
  height: number;
  data: Uint8Array;
};

const spritesCsvPath = process.argv[2] ?? "analysis/animal_horse/sprites.csv";
const atlasPlistPath =
  process.argv[3] ?? "data/animal_horse/animal_horse.plist";
const atlasPngPath = process.argv[4] ?? "data/animal_horse/animal_horse.png";
const outputPngPath =
  process.argv[5] ?? "analysis/animal_horse/animal_horse_atlas_regions.png";

const colors: Rgba[] = [
  [255, 60, 60, 255],
  [60, 190, 255, 255],
  [80, 230, 120, 255],
  [255, 210, 70, 255],
  [220, 100, 255, 255],
  [255, 130, 40, 255],
];
const backgroundColor: Rgba = [255, 0, 255, 255];

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
  " ": ["000", "000", "000", "000", "000"],
  "A": ["010", "101", "111", "101", "101"],
  "D": ["110", "101", "101", "101", "110"],
  "E": ["111", "100", "111", "100", "111"],
  "F": ["111", "100", "111", "100", "100"],
  "G": ["111", "100", "101", "101", "111"],
  "I": ["111", "010", "010", "010", "111"],
  "L": ["100", "100", "100", "100", "111"],
  "M": ["101", "111", "111", "101", "101"],
  "N": ["101", "111", "111", "111", "101"],
  "P": ["111", "101", "111", "100", "100"],
  "R": ["110", "101", "110", "101", "101"],
  "S": ["111", "100", "111", "001", "111"],
  "T": ["111", "010", "010", "010", "010"],
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
): Pick<AtlasFrame, "x" | "y" | "width" | "height"> | undefined {
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

function readKeyString(body: string, key: string): string | undefined {
  const pattern = new RegExp(
    `<key>${key}</key>\\s*<string>([\\s\\S]*?)</string>`,
  );
  const match = pattern.exec(body);
  return match ? decodeXml(match[1]) : undefined;
}

function readKeyBoolean(body: string, key: string): boolean {
  const pattern = new RegExp(`<key>${key}</key>\\s*<(true|false)\\s*/>`);
  return pattern.exec(body)?.[1] === "true";
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
    frames.set(name, {
      name,
      ...rect,
      rotated:
        readKeyBoolean(body, "textureRotated") ||
        readKeyBoolean(body, "rotated"),
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
  if (bitDepth !== 8 || interlace !== 0 || ![2, 6].includes(colorType)) {
    throw new Error("Only non-interlaced 8-bit RGB/RGBA PNG files are supported");
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
      const left = x >= bytesPerPixel ? sourceData[rowStart + x - bytesPerPixel] : 0;
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
    data[targetIndex + 3] =
      colorType === 6 ? sourceData[sourceIndex + 3] : 255;
  }

  return { width, height, data };
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

function pngChunk(type: string, data: Buffer): Buffer {
  const typeBuffer = Buffer.from(type, "ascii");
  const length = Buffer.alloc(4);
  length.writeUInt32BE(data.length, 0);
  const crc = Buffer.alloc(4);
  crc.writeUInt32BE(crc32(Buffer.concat([typeBuffer, data])), 0);
  return Buffer.concat([length, typeBuffer, data, crc]);
}

function setPixel(image: RgbaImage, x: number, y: number, color: Rgba): void {
  if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
    return;
  }
  const index = (y * image.width + x) * 4;
  const alpha = color[3] / 255;
  if (alpha >= 1) {
    image.data[index] = color[0];
    image.data[index + 1] = color[1];
    image.data[index + 2] = color[2];
    image.data[index + 3] = 255;
    return;
  }

  image.data[index] = Math.round(color[0] * alpha + image.data[index] * (1 - alpha));
  image.data[index + 1] = Math.round(
    color[1] * alpha + image.data[index + 1] * (1 - alpha),
  );
  image.data[index + 2] = Math.round(
    color[2] * alpha + image.data[index + 2] * (1 - alpha),
  );
  image.data[index + 3] = 255;
}

function fillRect(
  image: RgbaImage,
  x: number,
  y: number,
  width: number,
  height: number,
  color: Rgba,
): void {
  for (let py = y; py < y + height; py++) {
    for (let px = x; px < x + width; px++) {
      setPixel(image, px, py, color);
    }
  }
}

function drawLine(
  image: RgbaImage,
  x0: number,
  y0: number,
  x1: number,
  y1: number,
  color: Rgba,
): void {
  let dx = Math.abs(x1 - x0);
  const sx = x0 < x1 ? 1 : -1;
  let dy = -Math.abs(y1 - y0);
  const sy = y0 < y1 ? 1 : -1;
  let error = dx + dy;

  while (true) {
    setPixel(image, x0, y0, color);
    if (x0 === x1 && y0 === y1) {
      break;
    }
    const e2 = error * 2;
    if (e2 >= dy) {
      error += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      error += dx;
      y0 += sy;
    }
    dx = Math.abs(x1 - x0);
    dy = -Math.abs(y1 - y0);
  }
}

function drawStrokeRect(
  image: RgbaImage,
  x: number,
  y: number,
  width: number,
  height: number,
  color: Rgba,
  dashed: boolean,
): void {
  for (let offset = 0; offset < 2; offset++) {
    for (let px = x; px < x + width; px++) {
      if (!dashed || Math.floor((px - x) / 5) % 2 === 0) {
        setPixel(image, px, y + offset, color);
        setPixel(image, px, y + height - 1 - offset, color);
      }
    }
    for (let py = y; py < y + height; py++) {
      if (!dashed || Math.floor((py - y) / 5) % 2 === 0) {
        setPixel(image, x + offset, py, color);
        setPixel(image, x + width - 1 - offset, py, color);
      }
    }
  }
}

function drawText(
  image: RgbaImage,
  text: string,
  x: number,
  y: number,
  color: Rgba,
): void {
  let cursor = x;
  const scale = 2;
  for (const char of text.toUpperCase()) {
    const glyph = glyphs[char] ?? glyphs[" "];
    glyph.forEach((line, gy) => {
      [...line].forEach((pixel, gx) => {
        if (pixel !== "1") {
          return;
        }
        fillRect(
          image,
          cursor + gx * scale,
          y + gy * scale,
          scale,
          scale,
          color,
        );
      });
    });
    cursor += (Math.max(...glyph.map((line) => line.length)) + 1) * scale;
  }
}

function textWidth(text: string): number {
  const scale = 2;
  return [...text.toUpperCase()].reduce((width, char) => {
    const glyph = glyphs[char] ?? glyphs[" "];
    return width + (Math.max(...glyph.map((line) => line.length)) + 1) * scale;
  }, 0);
}

function copyImage(image: RgbaImage): RgbaImage {
  return {
    width: image.width,
    height: image.height,
    data: new Uint8Array(image.data),
  };
}

function flattenOnBackground(image: RgbaImage, background: Rgba): RgbaImage {
  const flattened: RgbaImage = {
    width: image.width,
    height: image.height,
    data: new Uint8Array(image.data.length),
  };

  for (let pixel = 0; pixel < image.width * image.height; pixel++) {
    const index = pixel * 4;
    const alpha = image.data[index + 3] / 255;
    flattened.data[index] = Math.round(
      image.data[index] * alpha + background[0] * (1 - alpha),
    );
    flattened.data[index + 1] = Math.round(
      image.data[index + 1] * alpha + background[1] * (1 - alpha),
    );
    flattened.data[index + 2] = Math.round(
      image.data[index + 2] * alpha + background[2] * (1 - alpha),
    );
    flattened.data[index + 3] = 255;
  }

  return flattened;
}

function renderOverlayPng(options: {
  atlas: RgbaImage;
  usedFrames: Map<string, CsvRow[]>;
  atlasFrames: Map<string, AtlasFrame>;
}): { image: RgbaImage; missingFrames: string[]; rotatedCount: number } {
  const { atlas, usedFrames, atlasFrames } = options;
  const image = flattenOnBackground(atlas, backgroundColor);
  const missingFrames: string[] = [];
  let rotatedCount = 0;
  let colorIndex = 0;

  for (const frameName of [...usedFrames.keys()].sort()) {
    const frame = atlasFrames.get(frameName);
    if (!frame) {
      missingFrames.push(frameName);
      continue;
    }

    const color = colors[colorIndex % colors.length];
    const rows = usedFrames.get(frameName) ?? [];
    const ids = rows.map((row) => row.nodeId).join("/");
    const label = `#${ids.length > 18 ? `${ids.slice(0, 18)}+` : ids}${
      frame.rotated ? " ROT" : ""
    }`;
    const textureWidth = frame.rotated ? frame.height : frame.width;
    const textureHeight = frame.rotated ? frame.width : frame.height;
    const labelWidth = Math.max(38, textWidth(label) + 4);
    const labelX = Math.max(0, Math.min(frame.x, image.width - labelWidth));
    const labelY = Math.max(20, frame.y - 14);

    if (frame.rotated) {
      rotatedCount++;
    }

    drawStrokeRect(
      image,
      frame.x,
      frame.y,
      textureWidth,
      textureHeight,
      color,
      frame.rotated,
    );
    if (frame.rotated) {
      const x = frame.x + textureWidth - 13;
      const y = frame.y + 4;
      drawLine(image, x, y, x + 8, y, color);
      drawLine(image, x + 8, y, x + 8, y + 8, color);
      drawLine(image, x + 8, y, x - 2, y + 10, color);
    }
    fillRect(image, labelX, labelY, labelWidth, 14, [0, 0, 0, 200]);
    drawText(image, label, labelX + 2, labelY + 2, color);
    colorIndex++;
  }

  const summary = `FRAMES ${usedFrames.size} PLIST ${atlasFrames.size} ROTATED ${rotatedCount} MISSING ${missingFrames.length}`;
  fillRect(image, 0, 0, image.width, 20, [0, 0, 0, 210]);
  drawText(image, summary, 4, 5, [255, 255, 255, 255]);

  return { image, missingFrames, rotatedCount };
}

function main(): void {
  const spritesCsv = resolve(spritesCsvPath);
  const rows = parseCsv(readFileSync(spritesCsv, "utf8"));
  const usedFrames = groupSpritesByFrame(rows, animationRowsForSprites(spritesCsv));
  const atlasFrames = readAtlasFrames(resolve(atlasPlistPath));
  includeRelatedAtlasFrames(usedFrames, atlasFrames);
  const atlas = readPng(resolve(atlasPngPath));
  const outputPng = resolve(outputPngPath);
  const { image, missingFrames, rotatedCount } = renderOverlayPng({
    atlas,
    usedFrames,
    atlasFrames,
  });

  writePng(outputPng, image);

  console.log(`Wrote ${outputPng}`);
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
  console.error(`atlas-region-overlay-png failed: ${message}`);
  process.exitCode = 1;
}
