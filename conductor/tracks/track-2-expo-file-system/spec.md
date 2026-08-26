# Track 2 — expo-file-system

## Status

Complete (file ops verified end-to-end on emulator via N-API promises).

## Goal

Port **`expo-file-system`** native operations onto the N-API bridge from Track 1, proving
the promise primitives and POSIX file APIs end to end.

## Scope

1. **Native module (`expo-file-system/ohos/`):**
   - `writeFile(path, contents)` → Promise<void>
   - `readFile(path)` → Promise<string>
   - `statFile(path)` → Promise<{size,isDirectory,isFile}>
   - `makeDirectory(path)` → Promise<void>
   - `listDirectory(path)` → Promise<string[]>
   - `deleteFile(path)` → Promise<void>
2. **Backend:** OpenHarmony libc POSIX (`open`/`read`/`write`/`stat`/`mkdir`/`opendir`/
   `readdir`/`unlink`), the C-level equivalent of `@ohos.file.fs`. Sandbox `filesDir`
   resolved in ArkTS via `getContext(this).filesDir`.
3. **Promise exercise:** `writeFile`/`readFile`/`statFile` etc. return real JS promises via
   `napi_create_promise` + deferred resolve/reject.

## Out of scope

- URI/sandbox semantics beyond `filesDir`, user-selected dirs (`@ohos.file.picker`).
- Download/upload progress events (EventEmitter) — added when the bridge consumes a real
  JS bundle.
- `expo-modules-core` JS shim (`requireNativeModule`).

## Exit criteria

1. `oniro-app build` succeeds.
2. Deploy/launch on the emulator; hilog shows `ExpoFileSystem` markers confirming
   write → read → stat → mkdir → list → delete.

## References

- `conductor/learning.md`.
- `expo-modules-core/ohos/` bridge primitives from Track 1.
