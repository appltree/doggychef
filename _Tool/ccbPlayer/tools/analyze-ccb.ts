import { existsSync, readdirSync } from "node:fs";
import { basename, resolve } from "node:path";
import { spawnSync } from "node:child_process";

type AnimalTarget = {
  key: string;
  name: string;
  dataDir: string;
  analysisDir: string;
  ccbPath: string;
};

const dataRoot = resolve("data");
const analysisRoot = resolve("analysis");
const requestedKeys = process.argv.slice(2);

function discoverTargets(): AnimalTarget[] {
  return readdirSync(dataRoot, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .flatMap((entry) => {
      const key = entry.name.replace(/^animal_/, "");
      const dataDir = resolve(dataRoot, entry.name);
      const preferredCcbPath = resolve(dataDir, `node_${entry.name}.ccb`);
      const ccbPaths = existsSync(preferredCcbPath)
        ? [preferredCcbPath]
        : readdirSync(dataDir)
            .filter((fileName) => /\.ccb$/i.test(fileName))
            .sort()
            .map((fileName) => resolve(dataDir, fileName));
      if (ccbPaths.length === 0) {
        return [];
      }
      return ccbPaths.map((ccbPath) => {
        const ccbName = basename(ccbPath).replace(/\.ccb$/i, "");
        const multiCcbAnalysisDir = resolve(analysisRoot, entry.name, ccbName);
        return {
          key,
          name: ccbPaths.length === 1 ? entry.name : `${entry.name}/${ccbName}`,
          dataDir,
          analysisDir: ccbPaths.length === 1 ? resolve(analysisRoot, entry.name) : multiCcbAnalysisDir,
          ccbPath,
        };
      });
    })
    .sort((left, right) => left.key.localeCompare(right.key));
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

function main(): void {
  const targets = selectTargets(discoverTargets());
  if (targets.length === 0) {
    throw new Error("No CCB files found under data/");
  }

  for (const target of targets) {
    console.log(`\n[ccb] ${target.name}`);
    runTool("tools/ccb-analyzer.ts", [target.ccbPath, target.analysisDir]);
  }
}

try {
  main();
} catch (error) {
  const message = error instanceof Error ? error.message : String(error);
  console.error(`analyze-ccb failed: ${message}`);
  process.exitCode = 1;
}
