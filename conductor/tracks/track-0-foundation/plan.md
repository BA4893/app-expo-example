# Track 0 Plan — Foundation & Build Spine

## Phases

### Phase 0 — Toolchain wiring

- Wire `hvigorw`, `ohpm`, and `hdc` (from DevEco Studio 6.1.1) into the layout
  `oniro-app` expects, via `ONIRO_CMD_TOOLS_PATH` or a corrected `~/command-line-tools`.
- Verify `oniro-app cmdtools status`, `oniro-app devices`, and a bare `hvigorw --version`.

### Phase 1 — Scaffold test container

- Scaffold a Native C++ OpenHarmony app under `apps/test-container/` (or project root).
- Configure `build-profile.json5` for SDK 6.0 (api 20), `runtimeOS: OpenHarmony`.

### Phase 2 — Native build spine

- Wire `oh-package.json5` and the CMake `CMakeLists.txt` for a `libentry.so`.
- Confirm the native library compiles and links as part of the HAP build.

### Phase 3 — Metro ↔ hvigor orchestration

- Implement `scripts/harmony-build.mjs` (Metro bundle → rawfile copy → `oniro-app build`).
- Implement `scripts/harmony-run.mjs` (Metro dev server + `oniro-app app apply/launch`).
- Document the rawfile bundle path consumed at runtime.

### Phase 4 — Verify

- `oniro-app build` succeeds.
- Deploy + launch on a connected device/emulator.
- Observe a JS `console` marker in hilog via `oniro-app watch`.

## Checkpointing

- Commit per phase (default is per task; phases are small enough to group logically here).
- Final `oniro-app build` runs only after Phase 4, per Conductor directives.
