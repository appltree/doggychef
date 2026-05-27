import { createReadStream, existsSync, statSync } from "node:fs";
import { createServer } from "node:http";
import { extname, join, normalize, resolve } from "node:path";
import { spawn, type ChildProcess } from "node:child_process";

const root = resolve(".");
const port = Number.parseInt(process.env.PORT ?? process.argv[2] ?? "8766", 10);
const watchBuild = process.env.CCB_PLAYER_WATCH !== "0";

const mimeTypes = new Map([
  [".html", "text/html; charset=utf-8"],
  [".css", "text/css; charset=utf-8"],
  [".js", "text/javascript; charset=utf-8"],
  [".mjs", "text/javascript; charset=utf-8"],
  [".json", "application/json; charset=utf-8"],
  [".ccb", "application/xml; charset=utf-8"],
  [".plist", "application/xml; charset=utf-8"],
  [".png", "image/png"],
  [".svg", "image/svg+xml; charset=utf-8"],
]);

function resolveRequestPath(urlPath: string): string | undefined {
  const decoded = decodeURIComponent(urlPath.split("?")[0] ?? "/");
  const relativePath = decoded === "/" ? "public/index.html" : decoded.replace(/^\/+/, "");
  const filePath = normalize(join(root, relativePath));
  if (!filePath.startsWith(root)) {
    return undefined;
  }
  if (existsSync(filePath) && statSync(filePath).isDirectory()) {
    return join(filePath, "index.html");
  }
  return filePath;
}

const server = createServer((request, response) => {
  const filePath = resolveRequestPath(request.url ?? "/");
  if (!filePath || !existsSync(filePath) || !statSync(filePath).isFile()) {
    response.writeHead(404, { "content-type": "text/plain; charset=utf-8" });
    response.end("Not found");
    return;
  }

  response.writeHead(200, {
    "content-type": mimeTypes.get(extname(filePath).toLowerCase()) ?? "application/octet-stream",
  });
  createReadStream(filePath).pipe(response);
});

let tscWatch: ChildProcess | undefined;
if (watchBuild) {
  const tscBin = resolve("node_modules", "typescript", "bin", "tsc");
  tscWatch = spawn(process.execPath, [tscBin, "-p", "tsconfig.app.json", "--watch", "--preserveWatchOutput"], {
    cwd: root,
    stdio: "inherit",
  });
}

server.listen(port, () => {
  console.log(`CCB Player: http://localhost:${port}/public/index.html`);
  if (watchBuild) {
    console.log("TypeScript watch build is running. Set CCB_PLAYER_WATCH=0 to serve without watching.");
  }
});

function shutdown(): void {
  tscWatch?.kill();
  server.close(() => process.exit(0));
}

process.on("SIGINT", shutdown);
process.on("SIGTERM", shutdown);
