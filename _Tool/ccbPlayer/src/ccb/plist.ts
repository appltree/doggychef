export type PlistValue = null | boolean | number | string | PlistValue[] | PlistDict;
export type PlistDict = { [key: string]: PlistValue };

type Token =
  | { kind: "open"; name: "dict" | "array" }
  | { kind: "close"; name: "dict" | "array" }
  | { kind: "key"; value: string }
  | { kind: "value"; value: PlistValue };

export function decodeXml(text: string): string {
  return text
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'")
    .replace(/&amp;/g, "&");
}

function tokenizePlist(xml: string): Token[] {
  const tokens: Token[] = [];
  const pattern =
    /<(dict|array)\s*\/>|<(dict|array)>|<\/(dict|array)>|<key>([\s\S]*?)<\/key>|<string>([\s\S]*?)<\/string>|<integer>([\s\S]*?)<\/integer>|<real>([\s\S]*?)<\/real>|<(true|false)\s*\/>/g;

  for (const match of xml.matchAll(pattern)) {
    if (match[1] === "dict") {
      tokens.push({ kind: "value", value: {} });
    } else if (match[1] === "array") {
      tokens.push({ kind: "value", value: [] });
    } else if (match[2] === "dict" || match[2] === "array") {
      tokens.push({ kind: "open", name: match[2] });
    } else if (match[3] === "dict" || match[3] === "array") {
      tokens.push({ kind: "close", name: match[3] });
    } else if (match[4] !== undefined) {
      tokens.push({ kind: "key", value: decodeXml(match[4]) });
    } else if (match[5] !== undefined) {
      tokens.push({ kind: "value", value: decodeXml(match[5]) });
    } else if (match[6] !== undefined) {
      tokens.push({ kind: "value", value: Number.parseInt(match[6], 10) });
    } else if (match[7] !== undefined) {
      tokens.push({ kind: "value", value: Number.parseFloat(match[7]) });
    } else if (match[8] !== undefined) {
      tokens.push({ kind: "value", value: match[8] === "true" });
    }
  }

  return tokens;
}

export function parsePlist(xml: string): PlistValue {
  const tokens = tokenizePlist(xml);
  let index = 0;

  function next(): Token {
    const token = tokens[index++];
    if (!token) {
      throw new Error("Unexpected end of plist");
    }
    return token;
  }

  function parseValue(): PlistValue {
    const token = next();
    if (token.kind === "value") {
      return token.value;
    }
    if (token.kind !== "open") {
      throw new Error(`Expected plist value, got ${token.kind}`);
    }
    if (token.name === "array") {
      const values: PlistValue[] = [];
      while (tokens[index]?.kind !== "close") {
        values.push(parseValue());
      }
      const close = next();
      if (close.kind !== "close" || close.name !== "array") {
        throw new Error("Array close token mismatch");
      }
      return values;
    }

    const dict: PlistDict = {};
    while (tokens[index]?.kind !== "close") {
      const key = next();
      if (key.kind !== "key") {
        throw new Error(`Expected dict key, got ${key.kind}`);
      }
      dict[key.value] = parseValue();
    }
    const close = next();
    if (close.kind !== "close" || close.name !== "dict") {
      throw new Error("Dict close token mismatch");
    }
    return dict;
  }

  return parseValue();
}

export function asDict(value: PlistValue | undefined, label: string): PlistDict {
  if (!value || Array.isArray(value) || typeof value !== "object") {
    throw new Error(`Expected ${label} to be a dictionary`);
  }
  return value;
}

export function asArray(value: PlistValue | undefined): PlistValue[] {
  return Array.isArray(value) ? value : [];
}

export function asNumber(value: PlistValue | undefined, fallback = 0): number {
  return typeof value === "number" ? value : fallback;
}

export function asString(value: PlistValue | undefined, fallback = ""): string {
  return typeof value === "string" ? value : fallback;
}

export function asBoolean(value: PlistValue | undefined, fallback = false): boolean {
  return typeof value === "boolean" ? value : fallback;
}

export function compact(value: PlistValue | undefined): string {
  if (value === undefined || value === null) {
    return "";
  }
  if (Array.isArray(value)) {
    return `[${value.map(compact).join(", ")}]`;
  }
  if (typeof value === "object") {
    return `{${Object.entries(value)
      .map(([key, entry]) => `${key}: ${compact(entry)}`)
      .join(", ")}}`;
  }
  return String(value);
}
