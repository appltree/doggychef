import { atlasFrameKey, makeFrameCanvas, type Atlas, type AtlasFrame } from "./atlas.js";
import {
  sampleNodePose,
  type CcbDocument,
  type CcbNode,
  type CcbSequence,
  type NodePose,
} from "./model.js";

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

export type Camera = {
  centerX: number;
  centerY: number;
  zoom: number;
};

const identity: Matrix = { a: 1, b: 0, c: 0, d: 1, e: 0, f: 0 };
const offstageThreshold = 250;

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

function nodeMatrix(pose: NodePose, contentSize: { width: number; height: number }): Matrix {
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
  if (pose.ignoreAnchorPointForPosition || (contentSize.width === 0 && contentSize.height === 0)) {
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

function normalizedAnchor(pose: NodePose): { x: number; y: number } {
  return {
    x: pose.anchorPoint.x >= 0 && pose.anchorPoint.x <= 1 ? pose.anchorPoint.x : 0.5,
    y: pose.anchorPoint.y >= 0 && pose.anchorPoint.y <= 1 ? pose.anchorPoint.y : 0.5,
  };
}

function isOffstagePose(pose: NodePose): boolean {
  return Math.abs(pose.position.x) > offstageThreshold || Math.abs(pose.position.y) > offstageThreshold;
}

function transformPoint(matrix: Matrix, x: number, y: number): { x: number; y: number } {
  return {
    x: matrix.a * x + matrix.c * y + matrix.e,
    y: matrix.b * x + matrix.d * y + matrix.f,
  };
}

function expandBounds(bounds: Bounds | undefined, points: Array<{ x: number; y: number }>): Bounds {
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

export class CcbRenderer {
  private readonly frameCanvases = new Map<string, HTMLCanvasElement>();

  constructor(
    private readonly canvas: HTMLCanvasElement,
    private readonly document: CcbDocument,
    private readonly atlas: Atlas,
  ) {}

  resize(): void {
    const ratio = window.devicePixelRatio || 1;
    const rect = this.canvas.getBoundingClientRect();
    const width = Math.max(1, Math.floor(rect.width * ratio));
    const height = Math.max(1, Math.floor(rect.height * ratio));
    if (this.canvas.width !== width || this.canvas.height !== height) {
      this.canvas.width = width;
      this.canvas.height = height;
    }
  }

  estimateBounds(sequence: CcbSequence): Bounds {
    let bounds: Bounds | undefined;
    const steps = Math.max(2, Math.ceil(sequence.length * 12));
    for (let index = 0; index <= steps; index++) {
      const time = (sequence.length * index) / steps;
      bounds = this.walkBounds(this.document.root, sequence, time, identity, true, bounds);
    }
    return bounds ?? { minX: -120, minY: -120, maxX: 120, maxY: 120 };
  }

  fitCamera(sequence: CcbSequence): Camera {
    const bounds = this.estimateBounds(sequence);
    const width = Math.max(1, bounds.maxX - bounds.minX);
    const height = Math.max(1, bounds.maxY - bounds.minY);
    const padding = 0.82;
    return {
      centerX: (bounds.minX + bounds.maxX) / 2,
      centerY: (bounds.minY + bounds.maxY) / 2,
      zoom: Math.min(this.canvas.width / width, this.canvas.height / height) * padding,
    };
  }

  draw(sequence: CcbSequence, time: number, camera: Camera): void {
    this.resize();
    const ctx = this.canvas.getContext("2d");
    if (!ctx) {
      return;
    }
    ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
    ctx.save();
    ctx.translate(this.canvas.width / 2, this.canvas.height / 2);
    ctx.scale(camera.zoom, -camera.zoom);
    ctx.translate(-camera.centerX, -camera.centerY);
    if (this.isSyntheticMirroredWalkout(sequence)) {
      ctx.translate(camera.centerX, 0);
      ctx.scale(-1, 1);
      ctx.translate(-camera.centerX, 0);
    }
    this.drawNode(ctx, this.document.root, sequence, time, identity, true);
    ctx.restore();
  }

  private walkBounds(
    node: CcbNode,
    sequence: CcbSequence,
    time: number,
    parentMatrix: Matrix,
    parentVisible: boolean,
    bounds: Bounds | undefined,
  ): Bounds | undefined {
    const pose = sampleNodePose(node, this.poseSequenceId(node, sequence), time);
    const visible = parentVisible && pose.visible && !isOffstagePose(pose);
    const contentSize = this.contentSizeForPose(pose);
    const matrix = multiply(parentMatrix, nodeMatrix(pose, contentSize));
    if (visible && pose.displayFrame) {
      const frame = this.frameForPose(pose);
      if (frame) {
        const width = frame.width;
        const height = frame.height;
        bounds = expandBounds(bounds, [
          transformPoint(matrix, 0, 0),
          transformPoint(matrix, width, 0),
          transformPoint(matrix, 0, height),
          transformPoint(matrix, width, height),
        ]);
      }
    }
    for (const child of node.children) {
      bounds = this.walkBounds(child, sequence, time, matrix, visible, bounds);
    }
    return bounds;
  }

  private drawNode(
    ctx: CanvasRenderingContext2D,
    node: CcbNode,
    sequence: CcbSequence,
    time: number,
    parentMatrix: Matrix,
    parentVisible: boolean,
  ): void {
    const pose = sampleNodePose(node, this.poseSequenceId(node, sequence), time);
    const visible = parentVisible && pose.visible && !isOffstagePose(pose);
    const matrix = multiply(parentMatrix, nodeMatrix(pose, this.contentSizeForPose(pose)));

    if (visible && pose.displayFrame) {
      this.drawSprite(ctx, pose, matrix);
    }
    for (const child of node.children) {
      this.drawNode(ctx, child, sequence, time, matrix, visible);
    }
  }

  private drawSprite(ctx: CanvasRenderingContext2D, pose: NodePose, matrix: Matrix): void {
    const frameName = pose.displayFrame?.frameName;
    if (!frameName) {
      return;
    }
    const frame = this.frameForPose(pose);
    if (!frame) {
      return;
    }
    const key = atlasFrameKey(frame.plistRef, frame.name);
    let frameCanvas = this.frameCanvases.get(key);
    if (!frameCanvas) {
      frameCanvas = makeFrameCanvas(this.atlas, frame);
      this.frameCanvases.set(key, frameCanvas);
    }
    const width = frameCanvas.width;
    const height = frameCanvas.height;
    const flipX = pose.flipX ? -1 : 1;
    const flipY = pose.flipY ? -1 : 1;

    ctx.save();
    ctx.transform(matrix.a, matrix.b, matrix.c, matrix.d, matrix.e, matrix.f);
    if (pose.flipX || pose.flipY) {
      ctx.translate(pose.flipX ? width : 0, pose.flipY ? height : 0);
      ctx.scale(flipX, flipY);
    }
    ctx.scale(1, -1);
    ctx.drawImage(frameCanvas, 0, -height, width, height);
    ctx.restore();
  }

  private contentSizeForPose(pose: NodePose): { width: number; height: number } {
    const frame = this.frameForPose(pose);
    return frame
      ? {
          width: frame.width,
          height: frame.height,
        }
      : { width: 0, height: 0 };
  }

  private frameForPose(pose: NodePose): AtlasFrame | undefined {
    const frameName = pose.displayFrame?.frameName;
    if (!frameName) {
      return undefined;
    }
    const plistRef = pose.displayFrame?.atlasPlist ?? "";
    return this.atlas.frames.get(atlasFrameKey(plistRef, frameName)) ?? this.atlas.frames.get(frameName);
  }

  private poseSequenceId(node: CcbNode, sequence: CcbSequence): number {
    const walkin = this.walkinFallbackSequence(sequence);
    return walkin && node.depth >= 2 ? walkin.sequenceId : sequence.sequenceId;
  }

  private walkinFallbackSequence(sequence: CcbSequence): CcbSequence | undefined {
    if (sequence.name !== "walkout") {
      return undefined;
    }
    const hasSpriteTracks = this.document.nodes.some((node) => {
      const tracks = node.animatedProperties.get(sequence.sequenceId);
      return node.depth >= 2 && tracks && tracks.size > 0;
    });
    return hasSpriteTracks ? undefined : this.document.sequences.find((entry) => entry.name === "walkin");
  }

  private isSyntheticMirroredWalkout(sequence: CcbSequence): boolean {
    return Boolean(this.walkinFallbackSequence(sequence));
  }
}
