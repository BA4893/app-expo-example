# Track 4 Plan — Expo JS Runtime Layer

## Phases

### Phase 0 — Native host object

- Add a `globalThis.expo.modules` N-API host object that exposes the registered native
  modules (`ExpoDevice`, `ExpoFileSystem`) as plain JS objects.
- Add `globalThis.expo.NativeModule` / `EventEmitter` / `SharedObject` / `SharedRef`
  class shims via `napi_run_script`.

### Phase 1 — JS shims

- Provide `requireNativeModule` / `requireOptionalNativeModule` reading from
  `globalThis.expo.modules`.
- Embed a small JS bootstrap that installs these on the global before module load.

### Phase 2 — Verify

- `oniro-app build`.
- Deploy + launch on emulator; confirm `requireNativeModule(...)` calls hit the native
  modules (hilog).

## Checkpointing

- Commit per phase.
- Final `oniro-app build` runs after Phase 2 only.
