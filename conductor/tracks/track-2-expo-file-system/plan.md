# Track 2 Plan — expo-file-system

## Phases

### Phase 0 — Native module

- Add `expo-file-system/ohos/` (`ExpoFileSystem.h` / `.cpp`) using libc POSIX APIs.
- Register under `ExpoFileSystem` in `napi_init.cpp`.
- Add sources + include dir to `CMakeLists.txt`.

### Phase 1 — Promise + ArkTS consumer

- Extend `Promise.h/.cpp` with `PromiseResolved`/`PromiseRejected` using
  `napi_create_promise` + deferred.
- Wire `Index.ets` to exercise the full write/read/stat/mkdir/list/delete cycle in
  `getContext(this).filesDir`.

### Phase 2 — Verify

- `oniro-app build`.
- Deploy + launch on emulator; confirm `ExpoFileSystem` hilog markers.

## Checkpointing

- Commit per phase.
- Final `oniro-app build` runs after Phase 2 only.
