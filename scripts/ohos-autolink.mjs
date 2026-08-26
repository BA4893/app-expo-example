#!/usr/bin/env node
/**
 * OpenHarmony autolinking resolver.
 *
 * Scans a consuming project's oh-package.json5 for `@expo/ohos-*` dependencies and
 * resolves each to its local package directory (containing index.ets, index.d.ts, and
 * native C++ sources). This is the OpenHarmony analogue of `expo-modules-autolinking`.
 *
 * Usage:
 *   node scripts/ohos-autolink.mjs [--project <dir>] [--json]
 *
 * Without --json it prints a human-readable summary; with --json it emits a machine-
 * readable map of package name -> resolved absolute path.
 */

import { readFileSync, existsSync } from "node:fs";
import { resolve, dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const SDK_ROOT = resolve(__dirname, "..");

function parseArgs(argv) {
  const args = { project: process.cwd(), json: false };
  for (let i = 0; i < argv.length; i++) {
    if (argv[i] === "--project" && argv[i + 1]) {
      args.project = resolve(argv[i + 1]);
      i++;
    } else if (argv[i] === "--json") {
      args.json = true;
    }
  }
  return args;
}

// Minimal JSON5 handling: strip // and /* */ comments and trailing commas.
function stripJson5Comments(text) {
  let out = "";
  let inString = false;
  let quote = "";
  let inLineComment = false;
  let inBlockComment = false;
  for (let i = 0; i < text.length; i++) {
    const c = text[i];
    const next = text[i + 1];
    if (inLineComment) {
      if (c === "\n") {
        inLineComment = false;
        out += c;
      }
      continue;
    }
    if (inBlockComment) {
      if (c === "*" && next === "/") {
        inBlockComment = false;
        i++;
      }
      continue;
    }
    if (inString) {
      out += c;
      if (c === quote && text[i - 1] !== "\\") inString = false;
      continue;
    }
    if (c === '"' || c === "'") {
      inString = true;
      quote = c;
      out += c;
      continue;
    }
    if (c === "/" && next === "/") {
      inLineComment = true;
      i++;
      continue;
    }
    if (c === "/" && next === "*") {
      inBlockComment = true;
      i++;
      continue;
    }
    out += c;
  }
  // Remove trailing commas (safe enough for the flat dependency objects we read).
  return out.replace(/,\s*([}\]])/g, "$1");
}

function readOhPackage(dir) {
  const p = join(dir, "oh-package.json5");
  if (!existsSync(p)) return null;
  return JSON.parse(stripJson5Comments(readFileSync(p, "utf8")));
}

function resolveExpoModules(projectDir) {
  const pkg = readOhPackage(projectDir);
  if (!pkg) return {};

  const deps = pkg.dependencies ?? {};
  const resolved = {};
  for (const [name, spec] of Object.entries(deps)) {
    if (!name.startsWith("@expo/ohos-")) continue;

    let path = null;
    if (typeof spec === "string") {
      if (spec.startsWith("file:")) {
        path = resolve(projectDir, spec.slice("file:".length));
      } else if (existsSync(join(SDK_ROOT, name.replace("@expo/ohos-", "")))) {
        // Local SDK checkout: @expo/ohos-expo-device -> ./expo-device
        path = join(SDK_ROOT, name.replace("@expo/ohos-", ""));
      }
    }
    resolved[name] = path;
  }
  return resolved;
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const modules = resolveExpoModules(args.project);

  if (args.json) {
    process.stdout.write(JSON.stringify(modules, null, 2) + "\n");
    return;
  }

  if (Object.keys(modules).length === 0) {
    process.stdout.write("No @expo/ohos-* modules found in this project.\n");
    return;
  }
  for (const [name, path] of Object.entries(modules)) {
    process.stdout.write(`${name} -> ${path ?? "(unresolved)"}\n`);
  }
}

main();
