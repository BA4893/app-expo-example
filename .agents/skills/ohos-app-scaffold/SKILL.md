---
name: ohos-app-scaffold
description: Scaffold a new OpenHarmony / HarmonyOS app (Standard ArkTS or Native C++) with the cross-platform `oniro-app` CLI and verify it builds. For the inner dev loop on an existing project use `ohos-app-dev`; for system/persistent bundles use `ohos-system-dev`.
---

# OHOS App Scaffold

Create a new app from a template, confirm it builds, then hand off. Everything goes through
the **`oniro-app` CLI**: cross-platform (Linux / macOS / Windows), non-interactive (explicit
flags, stdout=results / stderr=logs, non-zero exit on failure), and the owner of the canonical
project templates — so you never hand-roll a `cp` / `xcopy` template copy.

> **Prereqs:** `oniro-app` on PATH (`npm i -g @oniroproject/oniro-app`), plus an installed SDK
> and command-line tools (`oniro-app sdk install <ver>`, `oniro-app cmdtools install`).
> `oniro-app sdk list` / `oniro-app cmdtools status` confirm what's installed.

## Scaffold

`oniro-app templates list` shows the available templates. Scaffolding is one non-interactive,
cross-platform command:

```
oniro-app create --name <AppName> --bundle <com.example.app> --location <parent-dir> --sdk <api> [--template <id>]
```

- **Standard ArkTS (default):** omit `--template` — uses `EmptyAbility`, an ArkUI
  HelloWorld-style starter.
- **Native C++ (ArkTS + N-API):** add `--template NativeCpp` — an ArkUI app wired to a
  CMake-built native library (`libentry.so`, called from `Index.ets`).

`--sdk` is the numeric API level (run `oniro-app sdk list` for the version↔API mapping).
`--module <name>` renames the default `entry` module. `oniro-app create` writes the bundle
name, app name, and SDK version into the scaffold for you.

## Verify the build

```
oniro-app build          # auto-runs `ohpm install --all` if oh_modules/ is missing, then assembleHap
```
Resolve any errors before proceeding (see `ohos-app-dev/references/troubleshooting.md`). If
`build-profile.json5` shows the API level in parentheses (e.g. `6.0.0(20)`), that number **is**
the API level; `oniro-app sdk list` maps versions to API levels.

## After scaffolding

- On a successful build: `git init && git add -A && git commit -m "Initial commit"`.
- Hand off to **`ohos-app-dev`** for the inner loop (lint → build → deploy → run → logs → UI),
  or **`conductor-dev`** to add Conductor-based orchestration.
- ArkTS strict-mode rules and app architecture guidance live in **`ohos-app-dev`**
  (`references/arkts-strict.md`, `references/architecture.md`).

## Out of scope

- Inner-loop work on an existing project → `ohos-app-dev`.
- System / persistent bundles (systemui, launcher, OHOS source-tree builds) → `ohos-system-dev`.
