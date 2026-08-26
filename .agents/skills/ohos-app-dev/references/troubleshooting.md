# OpenHarmony App Troubleshooting (CLI-first)

All build/device actions go through the **`oniro-app` CLI**. Most failures surface on
**stderr** with a non-zero exit; read that first. Common cases:

## Build

### `oniro-app build` fails before compiling (toolchain not found)
- **Cause:** no SDK / command-line tools installed, or `oniro-app` can't locate them.
- **Fix:** `oniro-app sdk list` and `oniro-app cmdtools status` to check; install with
  `oniro-app sdk install <ver>` / `oniro-app cmdtools install`. `oniro-app build` probes the
  project-local `hvigorw` and transparently falls back to the cmd-tools `hvigorw` when a
  vendored wrapper is broken — you do not invoke `hvigorw` yourself.

### `oniro-app build` fails with an ArkTS or native compile error
- **Cause:** a source error — ArkTS strict mode, a missing dependency, or native/CMake.
- **Fix:**
  1. Read the failing task in the stderr log (e.g. `CompileArkTS`, `BuildNativeWithCmake`).
  2. `oniro-app lint --files <changed files>` catches ArkTS strict-mode violations early
     (see [`arkts-strict.md`](./arkts-strict.md)).
  3. Dependency change: edit `oh-package.json5`, then `oniro-app build` — it re-runs
     `ohpm install` when `oh_modules/` is missing (`--no-deps` to skip).

### "No signed HAPs found" / unsigned HAP
- **Cause:** no `signingConfigs` configured — `oniro-app build` does not sign.
- **Fix:** safe to ignore for a build-only check. To install on a device, generate signing
  material with `oniro-app sign`, then deploy via `ohos-app-dev`.

## SDK / API level

`oniro-app sdk list` shows installed SDKs and their API levels. If `build-profile.json5`
embeds the API in parentheses (e.g. `6.0.0(20)`), that number is the API level.

---

For the full inner-loop workflow (deploy, logs, UI) see the parent **`ohos-app-dev`** skill;
for system / persistent bundles see **`ohos-system-dev`**.
