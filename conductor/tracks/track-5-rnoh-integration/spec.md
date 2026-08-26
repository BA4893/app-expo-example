# Track 5 — RNOH Runtime Integration (React Native + Hermes + Metro)

## Status

Not started. Prerequisite: Track 4 complete + HarmonyOS NEXT toolchain + RNOH.

## Goal

Replace the N-API-only bridge with the full **React Native for OpenHarmony (RNOH)**
runtime stack — Hermes JS engine, Metro bundler, TurboModules, Fabric renderer, and
`requireNativeView` — so that a standard Expo JS bundle runs natively on HarmonyOS /
OpenHarmony without code changes.

## Background

Tracks 1–4 proved the **native module contract** end to end using OpenHarmony N-API
only. That path was chosen because:

1. OpenHarmony api 20 (the validated target) has no Hermes headers, no `facebook::jsi`,
   and no RNOH runtime.
2. RNOH targets **HarmonyOS NEXT** (API 21+), which requires Huawei Developer signing,
   a production-grade device, and a different SDK layout.
3. The N-API bridge is a clean, ArkTS-safe boundary that works today.

Track 5 bridges that gap: it layers the RNOH runtime on top of the proven native
modules, using the same module definitions from `expo-modules-core/ohos/`.

## Scope

1. **React Native / Hermes runtime**
   - Add `react-native` and `react-native-openharmony` to the JS dependency graph.
   - Create `package.json` with Metro, Hermes engine, and Expo-compatible scripts.
   - Port Hermes build to the OpenHarmony NDK (or consume prebuilt Hermes for OH).
   - Replace `napi_run_script` bootstrap with a `jsi::Runtime` + `HermesRuntime`
     initialization path that calls `InstallExpoGlobal` via JSI instead of N-API.

2. **Metro bundler / dev server**
   - Add `metro.config.js` with OpenHarmony asset extensions (`.ets`, `.hml`,
     `.js`, `.jsx`, `.ts`, `.tsx`).
   - Create `scripts/harmony-build.mjs` and `scripts/harmony-run.mjs` to sequence
     `metro bundle` → `oniro-app build` → `oniro-app app apply` → `oniro-app app launch`.
   - Wire Metro HMR to the OH app lifecycle (dev-only; release uses pre-bundled
     `rawfile` assets inside the HAP).

3. **TurboModules / Fabric / JSI bridge**
   - Replace the static N-API registry (`ExpoModulesRegistry`) with a TurboModule
     spec provider that emits `TurboModule` JS classes from the same C++ definitions.
   - Add `JsiTurboModuleFacade.{h,cpp}` — a compile-time switch: when `USE_JSI=1`
     (Hermes present), modules expose `jsi::HostObject` + `TurboModule` bindings;
     when `USE_JSI=0`, they fall back to the existing N-API path.
   - Add `FabricViewManagerFacade.{h,cpp}` skeleton that registers native view
     managers with the Fabric renderer (no-op under N-API).

4. **`requireNativeView` / view manager layer**
   - Add `expo-modules-core/js/view-manager.js` shim: `requireNativeView(ModuleName)`
     resolves to a Fabric `ViewManager` wrapper.
   - Add `expo-modules-core/js/turbomodule-shim.js` shim: mirrors Expo's
     `TurboModuleRegistry` / `createTurboModule` contract.
   - Add `expo-modules-core/js/fabric-shim.js` shim: mirrors Expo's
     `requireNativeComponent` / `Fabric` component contract.

## Out of scope

- Expo modules beyond the core contract (`expo-device`, `expo-file-system`).
- HarmonyOS NEXT production signing / AppGallery Connect distribution.
- HMS Kit integration beyond `deviceinfo` and POSIX file I/O.

## Exit criteria

1. `package.json` declares `react-native`, `react-native-openharmony`, Metro, and
   Hermes-engine dependencies.
2. `metro.config.js` builds a JS bundle the app can consume as a `rawfile` asset.
3. `JsiTurboModuleFacade` compiles in both `USE_JSI=0` (N-API fallback) and
   `USE_JSI=1` (Hermes) modes.
4. `FabricViewManagerFacade` compiles and registers view managers without crashing
   under N-API fallback.
5. `requireNativeModule`, `requireNativeView`, `TurboModuleRegistry`, and
   `requireNativeComponent` are all available on `globalThis` via `InstallExpoRuntime`.

## Prerequisites

- HarmonyOS NEXT SDK (API 21+) with Hermes + JSI headers.
- RNOH (`linhandev/ohos_react_native`) integrated into the build graph.
- Huawei Developer account for signing.
- A physical HarmonyOS NEXT device (emulator does not support RNOH at this time).

## References

- `conductor/learning.md` — rationale for N-API-first approach.
- Track 4 spec — JS runtime layer contract.
- RNOH docs: `https://gitee.com/openharmony-sig/ohos_react_native`
- Expo `expo-modules-core` JS source: `requireNativeModule.ts`, `NativeModule.ts`,
  `TurboModule.ts`, `requireNativeComponent.ts`.
