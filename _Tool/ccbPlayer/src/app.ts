import { assets, type AssetEntry } from "./ccb/assets.js";
import { loadAtlases, type Atlas } from "./ccb/atlas.js";
import { parseCcb, type CcbDocument, type CcbSequence } from "./ccb/model.js";
import { CcbRenderer, type Camera } from "./ccb/renderer.js";

type LoadedAsset = {
  entry: AssetEntry;
  document: CcbDocument;
  atlas: Atlas;
};

function requireElement<T extends Element>(selector: string): T {
  const element = document.querySelector<T>(selector);
  if (!element) {
    throw new Error(`Required DOM node is missing: ${selector}`);
  }
  return element;
}

const assetSelect = requireElement<HTMLSelectElement>("#assetSelect");
const sequenceSelect = requireElement<HTMLSelectElement>("#sequenceSelect");
const playButton = requireElement<HTMLButtonElement>("#playButton");
const restartButton = requireElement<HTMLButtonElement>("#restartButton");
const timeSlider = requireElement<HTMLInputElement>("#timeSlider");
const zoomSlider = requireElement<HTMLInputElement>("#zoomSlider");
const fitToggle = requireElement<HTMLInputElement>("#fitToggle");
const canvas = requireElement<HTMLCanvasElement>("#stageCanvas");
const assetMeta = requireElement<HTMLElement>("#assetMeta");
const timeText = requireElement<HTMLElement>("#timeText");
const fpsText = requireElement<HTMLElement>("#fpsText");
const statsText = requireElement<HTMLElement>("#statsText");

let loaded: LoadedAsset | undefined;
let renderer: CcbRenderer | undefined;
let activeSequence: CcbSequence | undefined;
let camera: Camera = { centerX: 0, centerY: 0, zoom: 1 };
let time = 0;
let playing = true;
let lastFrameTime = performance.now();
let fpsAccumulator = 0;
let fpsFrames = 0;
const fixedAnimalZoom = 1.5;
const fixedPivotCamera = { centerX: 0, centerY: 80 };

function preferredAssetKey(): string {
  const query = new URLSearchParams(location.search);
  return query.get("asset") ?? "horse";
}

function preferredSequenceName(): string | undefined {
  const query = new URLSearchParams(location.search);
  return query.get("sequence") ?? undefined;
}

function preferredTime(sequence: CcbSequence): number {
  const query = new URLSearchParams(location.search);
  const value = Number.parseFloat(query.get("time") ?? "0");
  return Number.isFinite(value)
    ? Math.max(0, Math.min(sequence.length, value))
    : 0;
}

function fillAssets(): void {
  assetSelect.replaceChildren(
    ...assets.map((asset) => {
      const option = document.createElement("option");
      option.value = asset.key;
      option.textContent = asset.label;
      return option;
    }),
  );
  assetSelect.value = assets.some((asset) => asset.key === preferredAssetKey())
    ? preferredAssetKey()
    : "horse";
}

function setMeta(message: string): void {
  assetMeta.textContent = message;
}

async function fetchText(url: string): Promise<string> {
  const response = await fetch(url);
  if (!response.ok) {
    throw new Error(`${response.status} ${response.statusText}: ${url}`);
  }
  return response.text();
}

function chooseSequence(document: CcbDocument): CcbSequence {
  const requested = preferredSequenceName();
  return (
    document.sequences.find((sequence) => sequence.name === requested) ??
    document.sequences.find((sequence) => sequence.autoPlay) ??
    document.sequences[0]
  );
}

function fillSequences(ccb: CcbDocument): void {
  sequenceSelect.replaceChildren(
    ...ccb.sequences.map((sequence) => {
      const option = window.document.createElement("option");
      option.value = String(sequence.sequenceId);
      option.textContent = `${sequence.name} (${sequence.length.toFixed(2)}s)`;
      return option;
    }),
  );
}

function updateStats(): void {
  if (!loaded || !activeSequence) {
    statsText.textContent = "";
    return;
  }
  const spriteCount = loaded.document.nodes.filter(
    (node) => node.baseClass === "CCSprite",
  ).length;
  statsText.textContent = [
    `fileType: ${loaded.document.fileType}`,
    `fileVersion: ${loaded.document.fileVersion}`,
    `nodes: ${loaded.document.nodes.length}`,
    `sprites: ${spriteCount}`,
    `atlas frames: ${loaded.atlas.frames.size}`,
    `sequence: ${activeSequence.name}`,
  ].join("\n");
}

function resetCamera(): void {
  if (!renderer || !activeSequence) {
    return;
  }
  camera = {
    centerX: fixedPivotCamera.centerX,
    centerY: fixedPivotCamera.centerY,
    zoom: fixedAnimalZoom * Number.parseFloat(zoomSlider.value),
  };
}

async function loadAsset(entry: AssetEntry): Promise<void> {
  playing = false;
  playButton.textContent = "Play";
  setMeta(`Loading ${entry.label}...`);
  const [ccbText, atlas] = await Promise.all([
    fetchText(entry.ccbUrl),
    loadAtlases(entry.atlases),
  ]);
  const document = parseCcb(ccbText);
  loaded = { entry, document, atlas };
  renderer = new CcbRenderer(canvas, document, atlas);
  fillSequences(document);
  activeSequence = chooseSequence(document);
  sequenceSelect.value = String(activeSequence.sequenceId);
  timeSlider.max = String(activeSequence.length);
  time = preferredTime(activeSequence);
  timeSlider.value = String(time);
  resetCamera();
  updateStats();
  setMeta(`${entry.label} loaded`);
  playing = new URLSearchParams(location.search).get("paused") !== "1";
  playButton.textContent = playing ? "Pause" : "Play";
}

function setSequence(sequenceId: number): void {
  if (!loaded) {
    return;
  }
  activeSequence =
    loaded.document.sequences.find(
      (sequence) => sequence.sequenceId === sequenceId,
    ) ?? loaded.document.sequences[0];
  time = 0;
  timeSlider.max = String(activeSequence.length);
  timeSlider.value = "0";
  resetCamera();
  updateStats();
}

function render(now: number): void {
  const delta = Math.min(0.05, (now - lastFrameTime) / 1000);
  lastFrameTime = now;

  if (playing && activeSequence) {
    time += delta;
    if (time > activeSequence.length) {
      const overflow = time % activeSequence.length;
      const chainedSequence = loaded?.document.sequences.find(
        (sequence) => sequence.sequenceId === activeSequence?.chainedSequenceId,
      );
      if (chainedSequence && activeSequence.chainedSequenceId >= 0) {
        activeSequence = chainedSequence;
        sequenceSelect.value = String(activeSequence.sequenceId);
        timeSlider.max = String(activeSequence.length);
        time = overflow;
        resetCamera();
        updateStats();
      } else if (activeSequence.chainedSequenceId < 0) {
        time = overflow;
      } else {
        time = activeSequence.length;
        playing = false;
        playButton.textContent = "Play";
      }
    }
    timeSlider.value = String(time);
  }

  if (renderer && activeSequence) {
    renderer.draw(activeSequence, time, camera);
  }

  if (activeSequence) {
    timeText.textContent = `${time.toFixed(3)}s / ${activeSequence.length.toFixed(3)}s`;
  }
  fpsAccumulator += delta;
  fpsFrames++;
  if (fpsAccumulator >= 0.4) {
    fpsText.textContent = `${Math.round(fpsFrames / fpsAccumulator)} fps`;
    fpsAccumulator = 0;
    fpsFrames = 0;
  }
  requestAnimationFrame(render);
}

assetSelect.addEventListener("change", () => {
  const entry = assets.find((asset) => asset.key === assetSelect.value);
  if (entry) {
    void loadAsset(entry).catch((error: unknown) =>
      setMeta(error instanceof Error ? error.message : String(error)),
    );
  }
});

sequenceSelect.addEventListener("change", () => {
  setSequence(Number.parseInt(sequenceSelect.value, 10));
});

playButton.addEventListener("click", () => {
  playing = !playing;
  playButton.textContent = playing ? "Pause" : "Play";
});

restartButton.addEventListener("click", () => {
  time = 0;
  timeSlider.value = "0";
});

timeSlider.addEventListener("input", () => {
  time = Number.parseFloat(timeSlider.value);
});

fitToggle.addEventListener("change", resetCamera);
zoomSlider.addEventListener("input", () => {
  if (!renderer || !activeSequence) {
    return;
  }
  resetCamera();
});

window.addEventListener("resize", resetCamera);

fillAssets();
void loadAsset(
  assets.find((asset) => asset.key === assetSelect.value) ?? assets[0],
).catch((error: unknown) =>
  setMeta(error instanceof Error ? error.message : String(error)),
);
requestAnimationFrame((now) => {
  lastFrameTime = now;
  requestAnimationFrame(render);
});
