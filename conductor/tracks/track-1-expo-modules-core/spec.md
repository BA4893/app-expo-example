# Track 1 — Expo Modules Core (N-API backend) + expo-device

## Status

Complete (N-API bridge + expo-device verified end-to-end on emulator).

## Direction (revised)

Build Expo's C++ interop layer against **OpenHarmony N-API directly** (`napi_env`,
`napi_value`), not against `facebook::jsi`/Hermes. RNOH (Hermes + Metro + `RNAbility`) is
deferred — it targets HarmonyOS NEXT (Huawei signing), which is a different build target
from the validated OpenHarmony api 20 / `oniro-app` toolchain.

## Goal

Prove the full JS → N-API → OpenHarmony system API → hilog round trip by porting
**`expo-device`** on top of a minimal, self-contained N-API module registry.

## Scope

1. **Bridge spine (C++):**
   - A static module registry (`ExpoModulesRegistry`) with an explicit
     `name → (method → napi_callback)` table. No reflection, no `any` — ArkTS strict-safe.
   - Promise + event primitives on `napi_env`: `napi_create_promise` /
     `napi_resolve_deferred` / `napi_reject_deferred` for async methods;
     `napi_call_function` for event emission.
   - A `AnyValue`-style tagged value model (`Undefined | Null | Bool | Number | String |
     ArrayBuffer`) mapped onto `napi_valuetype`.
2. **`expo-device` native side:**
   - Map `Device.modelName`, `brand`, `deviceType`, `osName`, `osVersion`,
     `totalMemory` onto `deviceinfo.h` (`libdeviceinfo_ndk.z.so`): `OH_GetProductModel`,
     `OH_GetBrand`, `OH_GetMarketName`, `OH_GetDeviceType`, `OH_GetOSFullName`,
     `OH_GetSdkApiVersion`.
   - Emit a hilog marker on each native call so the round trip is observable.
3. **Verification:**
   - `oniro-app build` succeeds.
   - Deploy/launch on the OpenHarmony emulator; confirm device values + hilog markers.

## Out of scope

- `expo-file-system` (Track 2).
- `expo-modules-core` JavaScript layer (`requireNativeModule`, `NativeModule` class) — this
  track is the **native** bridge only; the JS shim is added when the bridge is consumed by
  a real bundle (Track 2/3).
- RNOH / Hermes / Metro integration.

## Exit criteria

1. `oniro-app build` succeeds with the N-API bridge + `expo-device` compiled.
2. App deploys/launches on the emulator.
3. hilog shows `ExpoDevice` markers with correct values from `deviceinfo.h`, proving the
   JS → N-API → system API round trip.

## References

- `conductor/learning.md` — Track 1 direction note.
- Architecture plan: `~/.commandcode/plans/expo-sdk-openharmony-architecture.md` (§1.4).
