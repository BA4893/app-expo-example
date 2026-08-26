# Track 3 — Hardening, Packaging & Release

## Status

Complete (packages restructured, autolinker verified, docs/CI/tests added).

## Goal

Turn the working native modules (Track 1/2) into a **consumable SDK** that a downstream
OpenHarmony app can depend on via `oh-package.json5`, rather than requiring a source-level
checkout of `expo-modules-core`, `expo-device`, and `expo-file-system`.

## Scope

1. **Publishable HAR packages:** restructure each module as an ohpm-publishable HAR package
   (`oh-package.json5` + `build-profile.json5` + `hvigorfile.ts` with `harTasks`, an
   `index.ets`/`index.d.ts` surface, and native `.so`/`.cpp` sources). Root workspace
   `oh-package.json5` declares the three as local file dependencies.
2. **ohos autolinking resolver:** a small Node script (`scripts/ohos-autolink.mjs`) that
   scans the app's `oh-package.json5` for `expo-*` modules and resolves their `.har`
   artifacts (and CMake sources) into the consuming project — the OpenHarmony analogue of
   `expo-modules-autolinking`.
3. **CI:** a lint + build workflow that runs `oniro-app lint` and `oniro-app build` on
   every commit.
4. **Docs:** a top-level `README.md` with Expo-developer-oriented getting-started
   instructions (prereqs, toolchain, build, deploy, adding modules).
5. **Test coverage:** unit tests (Hypium) for the ArkTS-facing module surfaces plus a
   coverage target consistent with `conductor/workflow.md`.

## Out of scope

- Publishing to a public ohpm registry (no registry/auth available; local file deps +
  `ohpm publish`-ready metadata only).
- Fabric/TurboModule view components.
- Download/upload progress events (EventEmitter).

## Exit criteria

1. A downstream app can add the three modules via `oh-package.json5` local file refs and
   call `ExpoDevice` + `ExpoFileSystem` after `oniro-app build`.
2. `scripts/ohos-autolink.mjs` resolves module `.har`/source paths from a consuming
   project.
3. `oniro-app lint` and `oniro-app build` pass; CI workflow present.
4. Root `README.md` guides an Expo developer from zero to a running module call.

## References

- `conductor/learning.md`
- `ohos-app-dev` skill (lint/build/deploy).
- `conductor/workflow.md` (test coverage, lint-before-commit).
