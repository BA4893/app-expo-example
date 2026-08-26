# Track 3 Plan — Hardening, Packaging & Release

## Phases

### Phase 0 — Package structure

- Add `oh-package.json5`, `build-profile.json5`, `hvigorfile.ts` (harTasks), and
  `index.ets`/`index.d.ts` to `expo-modules-core`, `expo-device`, `expo-file-system`.
- Move shared native sources under each package's `src/main/cpp` so they build as HARs.
- Add a root `oh-package.json5` + `hvigorfile.ts` + `build-profile.json5` workspace.

### Phase 1 — Autolinking resolver

- Implement `scripts/ohos-autolink.mjs` to scan consuming `oh-package.json5` for `expo-*`
  modules and emit resolved `.har`/source paths.

### Phase 2 — CI, docs, tests

- Add CI workflow (`oniro-app lint` + `oniro-app build`).
- Write root `README.md`.
- Add Hypium unit tests for module surfaces.

### Phase 3 — Verify

- Build a downstream consumer against the local packages; confirm `ExpoDevice` +
  `ExpoFileSystem` resolve and run.

## Checkpointing

- Commit per phase.
- Final `oniro-app build` runs after Phase 3 only.
