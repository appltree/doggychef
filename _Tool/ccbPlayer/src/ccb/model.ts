import {
  asArray,
  asBoolean,
  asDict,
  asNumber,
  asString,
  parsePlist,
  type PlistDict,
  type PlistValue,
} from "./plist.js";

export type Vec2 = { x: number; y: number };

export type CcbProperty = {
  name: string;
  type: string;
  value: PlistValue;
  baseValue?: PlistValue;
};

export type Keyframe = {
  time: number;
  value: PlistValue;
  easingType: number;
};

export type AnimationTrack = {
  name: string;
  type: number;
  keyframes: Keyframe[];
};

export type CcbNode = {
  id: number;
  parentId: number | null;
  depth: number;
  path: string;
  baseClass: string;
  displayName: string;
  properties: Map<string, CcbProperty>;
  animatedProperties: Map<number, Map<string, AnimationTrack>>;
  children: CcbNode[];
};

export type CcbSequence = {
  sequenceId: number;
  name: string;
  length: number;
  resolution: number;
  autoPlay: boolean;
  chainedSequenceId: number;
};

export type CcbDocument = {
  fileType: string;
  fileVersion: number;
  nodes: CcbNode[];
  root: CcbNode;
  sequences: CcbSequence[];
};

export type NodePose = {
  position: Vec2;
  anchorPoint: Vec2;
  scale: Vec2;
  rotation: number;
  visible: boolean;
  ignoreAnchorPointForPosition: boolean;
  flipX: boolean;
  flipY: boolean;
  displayFrame?: SpriteFrameRef;
};

export type SpriteFrameRef = {
  atlasPlist: string;
  frameName: string;
};

export const keyframeTypes = {
  toggle: 1,
  degrees: 2,
  position: 3,
  spriteFrame: 7,
} as const;

function readProperties(nodeDict: PlistDict): Map<string, CcbProperty> {
  return new Map(
    asArray(nodeDict.properties).map((value) => {
      const dict = asDict(value, "node property");
      const property = {
        name: asString(dict.name),
        type: asString(dict.type),
        value: dict.value ?? null,
        baseValue: dict.baseValue,
      };
      return [property.name, property];
    }),
  );
}

function readTrack(value: PlistValue): AnimationTrack {
  const track = asDict(value, "animation track");
  return {
    name: asString(track.name),
    type: asNumber(track.type),
    keyframes: asArray(track.keyframes)
      .map((entry) => {
        const keyframe = asDict(entry, "keyframe");
        return {
          time: asNumber(keyframe.time),
          value: keyframe.value ?? null,
          easingType: asNumber(asDict(keyframe.easing, "keyframe easing").type, 1),
        };
      })
      .sort((left, right) => left.time - right.time),
  };
}

function readAnimatedProperties(nodeDict: PlistDict): Map<number, Map<string, AnimationTrack>> {
  const animated = new Map<number, Map<string, AnimationTrack>>();
  const source = asDict(nodeDict.animatedProperties ?? {}, "animatedProperties");
  for (const [sequenceId, sequenceValue] of Object.entries(source)) {
    const tracks = new Map<string, AnimationTrack>();
    for (const [propertyName, trackValue] of Object.entries(asDict(sequenceValue, "sequence tracks"))) {
      tracks.set(propertyName, readTrack(trackValue));
    }
    animated.set(Number.parseInt(sequenceId, 10), tracks);
  }
  return animated;
}

function walkNodes(rootNode: PlistDict): { root: CcbNode; nodes: CcbNode[] } {
  const nodes: CcbNode[] = [];
  let nextId = 0;

  function walk(nodeDict: PlistDict, parentId: number | null, depth: number, path: string): CcbNode {
    const node: CcbNode = {
      id: nextId++,
      parentId,
      depth,
      path,
      baseClass: asString(nodeDict.baseClass, "Unknown"),
      displayName: asString(nodeDict.displayName, ""),
      properties: readProperties(nodeDict),
      animatedProperties: readAnimatedProperties(nodeDict),
      children: [],
    };
    nodes.push(node);
    node.children = asArray(nodeDict.children).map((child, childIndex) =>
      walk(asDict(child, "child node"), node.id, depth + 1, `${path}.${childIndex}`),
    );
    return node;
  }

  const root = walk(rootNode, null, 0, "0");
  return { root, nodes };
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
    };
  });
}

export function parseCcb(xml: string): CcbDocument {
  const root = asDict(parsePlist(xml), "plist root");
  const nodeGraph = asDict(root.nodeGraph, "nodeGraph");
  const graph = walkNodes(nodeGraph);
  return {
    fileType: asString(root.fileType),
    fileVersion: asNumber(root.fileVersion),
    root: graph.root,
    nodes: graph.nodes,
    sequences: readSequences(root),
  };
}

export function propertyValue(node: CcbNode, name: string): PlistValue | undefined {
  return node.properties.get(name)?.value;
}

export function propertyBase(node: CcbNode, name: string): PlistValue | undefined {
  return node.properties.get(name)?.baseValue;
}

export function toVec2(value: PlistValue | undefined, fallback: Vec2): Vec2 {
  const entries = asArray(value);
  return {
    x: asNumber(entries[0], fallback.x),
    y: asNumber(entries[1], fallback.y),
  };
}

export function spriteFrameRef(value: PlistValue | undefined): SpriteFrameRef | undefined {
  const entries = asArray(value).map((entry) => asString(entry));
  const frameName = entries.find((entry) => entry.endsWith(".png")) ?? entries[1] ?? "";
  if (!frameName) {
    return undefined;
  }
  return {
    atlasPlist: entries.find((entry) => entry.endsWith(".plist")) ?? entries[0] ?? "",
    frameName,
  };
}

function lerp(left: number, right: number, t: number): number {
  return left + (right - left) * t;
}

function ease(progress: number, easingType: number): number {
  if (easingType === 0) {
    return 0;
  }
  return progress;
}

function sampleTrack(track: AnimationTrack, time: number, fallback: PlistValue | undefined): PlistValue {
  const keyframes = track.keyframes;
  if (keyframes.length === 0) {
    return fallback ?? null;
  }
  if (track.type === keyframeTypes.toggle) {
    const activeKeyframes = keyframes.filter((keyframe) => time >= keyframe.time).length;
    return activeKeyframes % 2 === 1;
  }
  if (keyframes.length === 1) {
    return keyframes[0].value;
  }
  if (time <= keyframes[0].time) {
    return keyframes[0].value;
  }
  const last = keyframes[keyframes.length - 1];
  if (time >= last.time) {
    return last.value;
  }

  const nextIndex = keyframes.findIndex((keyframe) => keyframe.time > time);
  const left = keyframes[Math.max(0, nextIndex - 1)];
  const right = keyframes[nextIndex];
  if (track.type === keyframeTypes.spriteFrame) {
    return left.value;
  }

  const span = right.time - left.time || 1;
  const progress = ease((time - left.time) / span, left.easingType);
  if (track.type === keyframeTypes.degrees) {
    return lerp(asNumber(left.value), asNumber(right.value), progress);
  }
  if (track.type === keyframeTypes.position) {
    const leftVec = toVec2(left.value, { x: 0, y: 0 });
    const rightVec = toVec2(right.value, leftVec);
    return [lerp(leftVec.x, rightVec.x, progress), lerp(leftVec.y, rightVec.y, progress)];
  }
  return left.value;
}

function sampleProperty(
  node: CcbNode,
  sequenceId: number,
  propertyName: string,
  time: number,
): PlistValue | undefined {
  const staticProperty = propertyValue(node, propertyName);
  const fallback = propertyBase(node, propertyName) ?? staticProperty;
  const sequenceTracks = node.animatedProperties.get(sequenceId);
  const track = sequenceTracks?.get(propertyName);
  if (!track && propertyName === "visible") {
    const visibleIsSequenced = [...node.animatedProperties.values()].some((tracks) => tracks.has("visible"));
    const hasSequenceTracks = Boolean(sequenceTracks && sequenceTracks.size > 0);
    if (visibleIsSequenced && !hasSequenceTracks && isOffstageFallbackNode(node)) {
      return false;
    }
  }
  return track ? sampleTrack(track, time, fallback) : fallback;
}

function isOffstageFallbackNode(node: CcbNode): boolean {
  const position = toVec2(propertyBase(node, "position") ?? propertyValue(node, "position"), { x: 0, y: 0 });
  return Math.abs(position.x) > 300 || Math.abs(position.y) > 300;
}

export function sampleNodePose(node: CcbNode, sequenceId: number, time: number): NodePose {
  const flip = asArray(propertyValue(node, "flip"));
  const scale = toVec2(propertyValue(node, "scale"), { x: 1, y: 1 });
  return {
    position: toVec2(sampleProperty(node, sequenceId, "position", time), { x: 0, y: 0 }),
    anchorPoint: toVec2(propertyValue(node, "anchorPoint"), { x: 0.5, y: 0.5 }),
    scale,
    rotation: asNumber(sampleProperty(node, sequenceId, "rotation", time), 0),
    visible: Boolean(sampleProperty(node, sequenceId, "visible", time) ?? true),
    ignoreAnchorPointForPosition: Boolean(propertyValue(node, "ignoreAnchorPointForPosition") ?? false),
    flipX: Boolean(flip[0]),
    flipY: Boolean(flip[1]),
    displayFrame: spriteFrameRef(sampleProperty(node, sequenceId, "displayFrame", time)),
  };
}
