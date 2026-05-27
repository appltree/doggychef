import { existsSync, readdirSync, statSync } from "node:fs";
import { basename, dirname, resolve } from "node:path";
import { spawnSync } from "node:child_process";

type AnimalTarget = {
  key: string;
  name: string;
  dataDir: string;
  analysisDir: string;
  spritesCsvPath: string;
  atlasName: string;
  plistPath: string;
  atlasPngPath: string;
  overlaySvgPath: string;
  overlayPngPath: string;
  framePreviewPngPath: string;
};

const dataRoot = resolve("data");
const analysisRoot = resolve("analysis");
const requestedKeys = process.argv.slice(2);

function findFilesRecursive(dir: string, fileName: string): string[] {
  if (!existsSync(dir)) {
    return [];
  }
  return readdirSync(dir, { withFileTypes: true }).flatMap((entry) => {
    const path = resolve(dir, entry.name);
    if (entry.isDirectory()) {
      return findFilesRecursive(path, fileName);
    }
    return entry.name === fileName ? [path] : [];
  });
}

function findSpritesCsv(analysisDir: string, atlasName: string): string {
  const directPath = resolve(analysisDir, "sprites.csv");
  if (existsSync(directPath)) {
    return directPath;
  }
  const candidates = findFilesRecursive(analysisDir, "sprites.csv");
  return (
    candidates.find((path) => basename(dirname(path)).toLowerCase().includes(atlasName.toLowerCase())) ??
    candidates[0] ??
    directPath
  );
}

function discoverTargets(): AnimalTarget[] {
  return readdirSync(dataRoot, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .flatMap((entry) => {
      const key = entry.name.replace(/^animal_/, "");
      const dataDir = resolve(dataRoot, entry.name);
      const analysisDir = resolve(analysisRoot, entry.name);
      return readdirSync(dataDir)
        .filter((fileName) => fileName.endsWith(".plist"))
        .filter((fileName) => existsSync(resolve(dataDir, fileName.replace(/\.plist$/i, ".png"))))
        .filter((fileName) => existsSync(resolve(dataDir, fileName)) && statSync(resolve(dataDir, fileName)).size > 0)
        .map((fileName) => {
          const atlasName = fileName.replace(/\.plist$/i, "");
          return {
            key,
            name: entry.name,
            dataDir,
            analysisDir,
            spritesCsvPath: findSpritesCsv(analysisDir, atlasName),
            atlasName,
            plistPath: resolve(dataDir, fileName),
            atlasPngPath: resolve(dataDir, `${atlasName}.png`),
            overlaySvgPath: resolve(analysisDir, `${atlasName}_atlas_regions.svg`),
            overlayPngPath: resolve(analysisDir, `${atlasName}_atlas_regions.png`),
            framePreviewPngPath: resolve(analysisDir, `${atlasName}_frame_preview.png`),
          };
        });
    })
    .sort((left, right) => left.key.localeCompare(right.key) || left.atlasName.localeCompare(right.atlasName));
}

function normalizeKey(value: string): string {
  return basename(value).replace(/^animal_/, "").toLowerCase();
}

function selectTargets(allTargets: AnimalTarget[]): AnimalTarget[] {
  if (requestedKeys.length === 0) {
    return allTargets;
  }

  const selected: AnimalTarget[] = [];
  for (const key of requestedKeys.map(normalizeKey)) {
      const target = allTargets.find((entry) => entry.key === key);
      if (!target) {
        const available = allTargets.map((entry) => entry.key).join(", ");
      throw new Error(`Unknown target '${key}'. Available: ${available}`);
    }
    selected.push(...allTargets.filter((entry) => entry.key === key));
  }
  return selected;
}

function runTool(toolPath: string, args: string[]): void {
  const result = spawnSync(
    process.execPath,
    ["--experimental-strip-types", "--no-warnings", toolPath, ...args],
    { stdio: "inherit" },
  );
  if (result.status !== 0) {
    throw new Error(`${toolPath} failed with exit code ${result.status ?? "unknown"}`);
  }
}

function hasInputs(target: AnimalTarget): boolean {
  const missing = [
    target.spritesCsvPath,
    target.plistPath,
    target.atlasPngPath,
  ].filter((path) => !existsSync(path));

  if (missing.length === 0) {
    return true;
  }

  console.log(`\n[atlas] ${target.key}`);
  console.log(`Skipped; missing ${missing.join(", ")}`);
  return false;
}

function main(): void {
  const targets = selectTargets(discoverTargets());
  if (targets.length === 0) {
    throw new Error("No atlas files found under data/");
  }

  for (const target of targets) {
    if (!hasInputs(target)) {
      continue;
    }

    console.log(`\n[atlas] ${target.key}/${target.atlasName}`);
    runTool("tools/atlas-region-overlay.ts", [
      target.spritesCsvPath,
      target.plistPath,
      target.atlasPngPath,
      target.overlaySvgPath,
    ]);
    runTool("tools/atlas-region-overlay-png.ts", [
      target.spritesCsvPath,
      target.plistPath,
      target.atlasPngPath,
      target.overlayPngPath,
    ]);
    runTool("tools/atlas-frame-preview.ts", [
      target.spritesCsvPath,
      target.plistPath,
      target.atlasPngPath,
      target.framePreviewPngPath,
    ]);
  }
}

try {
  main();
} catch (error) {
  const message = error instanceof Error ? error.message : String(error);
  console.error(`analyze-atlas failed: ${message}`);
  process.exitCode = 1;
}
