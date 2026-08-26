---
name: ohos-app-dev
description: Develop, build, deploy, and validate OpenHarmony / HarmonyOS applications on a connected device. Use for the inner dev loop (lint → build → deploy → run → inspect logs → UI validation) on an existing project. For project scaffolding use `ohos-app-scaffold`; for system/persistent bundles use `ohos-system-dev`.
---

# OpenHarmony App Dev

Inner-loop skill for working on an **existing** OpenHarmony / HarmonyOS app. Every
toolchain and device action goes through the **`oniro-app` CLI** — a cross-platform,
agent-designed wrapper over `hvigorw` / `ohpm` / `codelinter` / `hdc` / `uitest`:
explicit flags, results on **stdout**, progress/logs on **stderr**, non-zero exit on
failure. Drop to raw `hdc` / `aa` / `bm` only for device operations `oniro-app` does
not cover.

> **Prereqs:** `oniro-app` on PATH (`npm i -g @oniroproject/oniro-app`) plus an installed
> SDK and command-line tools (`oniro-app sdk install <ver>`, `oniro-app cmdtools install`).
> Most commands default to the current directory or accept a trailing `[project-dir]`.

## When to use

Trigger on: "build the app", "deploy / run it on device", "lint these files", "grab the
logs", "take a screenshot", "tap this button", "why is it crashing on launch". For
*creating* a project → `ohos-app-scaffold`. For **system / persistent bundles** (systemui,
launcher, OS-source-tree builds, reboot-to-reload) → `ohos-system-dev`.

## Directives

1. **CLI-first.** Use `oniro-app <cmd>` for build / deploy / device / logs / input /
   capture. It auto-resolves the device serial, the bundle + main ability (from
   `app.json5` / `module.json5`), and built HAP paths. Use raw `hdc`/`aa`/`bm` only when
   no `oniro-app` command fits — and prefer adding the recipe here over guessing.
2. **Lint before you iterate.** After editing `.ets` / `.ts` / `.cpp` / `.h`, run
   `oniro-app lint --files <changed globs>` (or `oniro-app lint` for the whole project).
   Resolve errors; justify warnings.
3. **Device serial.** `oniro-app devices`. If more than one target, set
   `ONIRO_DEVICE_SERIAL` (or pass `--device <serial>`). Never silently pick one.
4. **No destructive device ops without confirmation** — `app uninstall`, wiping `/data/...`,
   overwriting system files via `file send`.
5. **UI coordinates.** `oniro-app screenshot --grid` overlays a 10×10 grid with axes
   labelled 0.0–1.0 — read tap targets off the gridlines. `oniro-app input` takes
   **pixels**; multiply the grid fraction by the device resolution (printed on the
   screenshot's stderr, e.g. `360x720`), or read an element's exact center from
   `oniro-app dump layout` (`c=[x,y]` as 0–1 → × resolution).

## Inner-loop workflow

1. **Edit** sources.
2. **Lint** — `oniro-app lint --files <changed files>`.
3. **Build** — `oniro-app build` — auto-runs `ohpm install --all` when `oh_modules/` is
   missing, builds, then discovers HAPs. Flags: `--module <m>` / `--product <p>` /
   `--mode release` for non-default targets, `--json` to print the discovered HAP paths,
   `--no-deps` to skip the ohpm step.
4. **Device** — `oniro-app devices`; stop and tell the user if empty.
5. **Deploy** — `oniro-app app apply` — verified install: handles sign-info-inconsistent
   (uninstall+install for a normal app), asset-cache invalidation (reboot), and
   persistent-bundle restart, and prints `method` / pre+post pid. Use `oniro-app app
   install` for a plain `hdc install`.
6. **Launch** — `oniro-app app launch` (reads bundle + ability automatically; `--ability
   <name>` / `--module <m>` to target a specific one).
7. **Observe**
   - **Logs (bounded):** `oniro-app watch --log '<regex>' --for 5000` collects matches for
     a window; `oniro-app wait --log '<regex>' --timeout 30000 [--bundle <b>]` returns the
     instant a line matches (perfect for "did action X fire?"). Fallback:
     `hdc shell hilog -x | grep -E ...`.
   - **Screenshot:** `oniro-app screenshot --grid -o /tmp/s.jpg` then read `/tmp/s.jpg`.
     Use `--grid` to pick tap targets; omit it for full-res content. `--max-dim <px>`
     caps the longest side (default 1024).
   - **Transient UI / animations / boot:** `oniro-app screenshot --contact-sheet -o
     /tmp/cs.jpg` — captures a burst (default 8 frames, `--burst N` / `--interval ms`)
     into **one** tiled, index-labelled sheet and prints per-frame change diffs (0..1) on
     stdout. The highest-diff frame is where it changed — one image instead of N.
   - **Layout:** `oniro-app dump layout` — pruned tree, bounds/centers normalised 0–1;
     look up a target's center before tapping when the screenshot is ambiguous.
   - **Drive:** `oniro-app input --type click --x <px> --y <px>` (also `doubleClick`,
     `longClick`, `swipe`/`drag`/`fling` with `--x2 --y2 --speed`, `keyEvent --key
     Back|Home|Power`, `inputText --text '...'`). Held / multi-segment paths:
     `oniro-app gesture --waypoints '[{"x":..,"y":..,"t":..}]' [--hold-start ms --hold-end ms]`.
8. **Stop / clean up** — `oniro-app app stop <bundle>`.

## Recipes

### Quick rebuild & reinstall
`oniro-app lint --files <files>` → `oniro-app build` → `oniro-app app apply` →
`oniro-app app stop <bundle>` → `oniro-app app launch`.

### Diagnose a runtime crash
`oniro-app app launch` to reproduce → `oniro-app watch --log 'FATAL|SIGSEGV| E ' --for 8000`
(or `hdc shell hilog -x | grep -E 'FATAL|SIGSEGV'`). For native faults, list
`/data/log/faultlog/faultlogger` via `hdc shell ls …` and pull the entry with
`oniro-app file recv <remote> <local>`.

### UI validation against a design
`oniro-app screenshot --grid -o /tmp/s.jpg` → read & compare. Read `build-profile.json5`
for the target API level (table below) — some components render differently across levels.

### Dependency change
Edit `oh-package.json5` → `oniro-app build` (re-runs `ohpm install` when needed).

### Common raw-device ops (no dedicated command)
- List targets: `hdc list targets -v`
- Force-stop: `hdc shell aa force-stop <bundle>`  (or `oniro-app app stop <bundle>`)
- Push / pull: `oniro-app file send <local> <remote>` / `oniro-app file recv <remote> <local>`
- Set a system prop: `hdc shell param set <key> <value>`
- Reboot: `oniro-app reboot` (see `ohos-system-dev` for reboot-to-reload).

## SDK / API level reference

Run `oniro-app sdk list` for the version↔API-level mapping (and which SDKs are installed).
Read `build-profile.json5` for `compatibleSdkVersion` / `targetSdkVersion`; if the version
embeds the API level in parentheses (e.g. `6.0.0(20)`), that number **is** the API level.

## Out of scope

- Scaffolding / templating a fresh project → `ohos-app-scaffold`.
- System / persistent bundles, OHOS source-tree builds, sign-cert traps, reboot-to-reload
  → `ohos-system-dev`.

## References

App-engineering knowledge for ArkTS / ArkUI work — this skill is the canonical home; the
other skills point here rather than restating it:
- [`references/arkts-strict.md`](./references/arkts-strict.md) — ArkTS strict-mode rules the
  compiler / `oniro-app lint` enforce.
- [`references/architecture.md`](./references/architecture.md) — Clean-Architecture layering
  for ArkTS apps (Domain / Data / Presentation).
- [`references/troubleshooting.md`](./references/troubleshooting.md) — common `oniro-app`
  build / SDK failures and fixes.
