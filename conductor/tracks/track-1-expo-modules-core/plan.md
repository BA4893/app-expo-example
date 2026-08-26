# Track 1 Plan — Expo Modules Core (N-API) + expo-device

## Phases

### Phase 0 — Bridge spine

- Add `expo-modules-core/ohos/` C++ with:
  - `ExpoModulesRegistry` — static name→(method→napi_callback) table.
  - `JSValue` tagged value model over `napi_valuetype`.
  - `Promise` helper (`napi_create_promise`, deferred resolve/reject).
  - `EventEmitter` helper (`napi_call_function` over a subscriber list).
- Wire the `.so` into the container via `entry/CMakeLists.txt`.

### Phase 1 — expo-device

- `expo-device/ohos/` native module registered under `ExpoDevice`.
- Map device methods to `deviceinfo.h` (`libdeviceinfo_ndk.z.so`).
- Emit `ExpoDevice` hilog markers for each call.

### Phase 2 — Verify

- `oniro-app build`.
- Deploy + launch on the emulator.
- Confirm hilog markers and correct device values.

## Checkpointing

- Commit per phase.
- Final `oniro-app build` runs after Phase 2 only, per Conductor directives.
