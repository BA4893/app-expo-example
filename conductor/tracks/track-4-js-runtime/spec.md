# Track 4 — Expo JS Runtime Layer

## Status

In progress.

## Goal

Wire the **Expo JS runtime contract** so an Expo-style JS bundle can call
`requireNativeModule("ExpoDevice")` and `requireNativeModule("ExpoFileSystem")` against the
native modules built in Tracks 1–2. This delivers the "full Expo JS runtime experience" at
the native-bridge boundary, without a Hermes/JSI or Metro dev-server dependency (which the
OpenHarmony NDK does not expose).

## Scope

1. **`globalThis.expo.modules` host object** — populated natively via N-API with the
   registered `ExpoDevice` and `ExpoFileSystem` modules. This is the object
   `requireNativeModule` reads from.
2. **`globalThis.expo.NativeModule` / `EventEmitter` / `SharedObject` / `SharedRef`** —
   JS class shims matching Expo's `expo-modules-core` contract.
3. **`requireNativeModule` / `requireOptionalNativeModule`** — a JS shim that resolves
   from `globalThis.expo.modules` (with a `NativeModulesProxy` fallback).
4. **`napi_run_script` bootstrap** — run an embedded JS bundle at native startup so the
   shims are installed before any module is required.

## Out of scope

- Metro dev-server / HMR and `.hbc` bytecode (no Hermes on this toolchain).
- `requireNativeView`, Fabric/TurboModule view components.
- Actual `expo` package bundling (the shim is hand-written to match the contract).

## Exit criteria

1. `oniro-app build` succeeds.
2. On the emulator, a JS bundle calls `requireNativeModule("ExpoDevice").getModelName()`
   and `requireNativeModule("ExpoFileSystem").writeFile(...)` via `napi_run_script`.
3. hilog shows the native calls hit the C++ modules.

## References

- `expo-modules-core` source: `requireNativeModule.ts`, `NativeModule.ts`,
  `ts-declarations/global.ts`.
- `conductor/learning.md`.
