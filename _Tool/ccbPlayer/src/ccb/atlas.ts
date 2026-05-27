import { asDict, asString, parsePlist, type PlistDict } from "./plist.js";

export type AtlasFrame = {
  name: string;
  plistRef: string;
  x: number;
  y: number;
  width: number;
  height: number;
  rotated: boolean;
  sourceWidth: number;
  sourceHeight: number;
};

export type Atlas = {
  frames: Map<string, AtlasFrame>;
  images: Map<string, HTMLImageElement>;
};

function parseRect(value: string): { x: number; y: number; width: number; height: number } | undefined {
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

function parseSize(value: string): { width: number; height: number } | undefined {
  const match = /\{\s*(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\s*\}/.exec(value);
  return match
    ? {
        width: Number.parseFloat(match[1]),
        height: Number.parseFloat(match[2]),
      }
    : undefined;
}

function readFrame(plistRef: string, name: string, dict: PlistDict): AtlasFrame | undefined {
  const rectValue = asString(dict.textureRect) || asString(dict.frame);
  const rect = parseRect(rectValue);
  if (!rect) {
    return undefined;
  }
  const rotated = Boolean(dict.textureRotated ?? dict.rotated ?? false);
  const sourceSize = parseSize(asString(dict.spriteSourceSize) || asString(dict.sourceSize)) ?? {
    width: rotated ? rect.height : rect.width,
    height: rotated ? rect.width : rect.height,
  };
  return {
    name,
    plistRef,
    ...rect,
    rotated,
    sourceWidth: sourceSize.width,
    sourceHeight: sourceSize.height,
  };
}

function frameKey(plistRef: string, frameName: string): string {
  return `${plistRef}::${frameName}`;
}

export function parseAtlasPlist(xml: string, plistRef = ""): Map<string, AtlasFrame> {
  if (!xml.trim()) {
    return new Map();
  }
  const root = asDict(parsePlist(xml), "atlas root");
  const framesDict = asDict(root.frames, "atlas frames");
  const frames = new Map<string, AtlasFrame>();
  for (const [name, value] of Object.entries(framesDict)) {
    const frame = readFrame(plistRef, name, asDict(value, "atlas frame"));
    if (frame) {
      frames.set(frameKey(plistRef, name), frame);
      if (!frames.has(name)) {
        frames.set(name, frame);
      }
    }
  }
  return frames;
}

export function loadImage(url: string): Promise<HTMLImageElement> {
  return new Promise((resolve, reject) => {
    const image = new Image();
    image.onload = () => resolve(image);
    image.onerror = () => reject(new Error(`Failed to load image: ${url}`));
    image.src = url;
  });
}

export async function loadAtlas(plistUrl: string, imageUrl: string): Promise<Atlas> {
  const [plistText, image] = await Promise.all([fetch(plistUrl).then((response) => response.text()), loadImage(imageUrl)]);
  return {
    frames: parseAtlasPlist(plistText, plistUrl),
    images: new Map([[plistUrl, image]]),
  };
}

export async function loadAtlases(entries: Array<{ plistRef: string; plistUrl: string; imageUrl: string }>): Promise<Atlas> {
  const loaded = await Promise.all(
    entries.map(async (entry) => {
      const [plistText, image] = await Promise.all([
        fetch(entry.plistUrl).then((response) => response.text()),
        loadImage(entry.imageUrl),
      ]);
      return { ...entry, frames: parseAtlasPlist(plistText, entry.plistRef), image };
    }),
  );
  const frames = new Map<string, AtlasFrame>();
  const images = new Map<string, HTMLImageElement>();
  for (const entry of loaded) {
    images.set(entry.plistRef, entry.image);
    for (const [key, frame] of entry.frames) {
      frames.set(key, frame);
    }
  }
  return { frames, images };
}

export function atlasFrameKey(plistRef: string, frameName: string): string {
  return frameKey(plistRef, frameName);
}

export function makeFrameCanvas(atlas: Atlas, frame: AtlasFrame): HTMLCanvasElement {
  const canvas = document.createElement("canvas");
  canvas.width = Math.max(1, Math.round(frame.width));
  canvas.height = Math.max(1, Math.round(frame.height));
  const ctx = canvas.getContext("2d");
  if (!ctx) {
    return canvas;
  }
  const image = atlas.images.get(frame.plistRef);
  if (!image) {
    return canvas;
  }
  if (frame.rotated) {
    ctx.translate(0, canvas.height);
    ctx.rotate(-Math.PI / 2);
    ctx.drawImage(image, frame.x, frame.y, frame.height, frame.width, 0, 0, frame.height, frame.width);
  } else {
    ctx.drawImage(image, frame.x, frame.y, frame.width, frame.height, 0, 0, frame.width, frame.height);
  }
  return canvas;
}
