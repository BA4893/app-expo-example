# Track 0 — Foundation & Build Spine

## Status

Complete (build spine + inner loop verified on emulator; RNOH runtime integration deferred
to Track 1).

## Goal

Stand up a minimal **react-native-openharmony (RNOH)** test-container app that boots a
JavaScript bundle on OpenHarmony and emits a `console` line to **hilog**, proving the
Metro → hvigor → hilog pipeline end to end before any Expo code is ported.

## Scope

- Scaffold a native OpenHarmony app (Native C++ template) as the test container.
- Wire the build toolchain (`ohpm` + `hvigor` + `hdc`) into the `oniro-app` layout.
- Add the CMake/NAPI native build spine.
- Create cross-platform orchestration scripts (`scripts/harmony-build.mjs`,
  `scripts/harmony-run.mjs`) that sequence Metro bundling with `oniro-app build`.
- Confirm the app deploys, launches, and a JS `console` line reaches hilog.

## Out of scope

- Expo Modules API / JSI bridge (Track 1).
- Any `expo-*` module port.
- Release packaging / autolinking (Track 3).

## Exit criteria

1. `oniro-app build` succeeds on the test container.
2. App deploys (`oniro-app app apply`) and launches (`oniro-app app launch`) on a connected
   device/emulator.
3. A JS `console` line is observed via `oniro-app watch --log '<marker>'`.

## References

- `conductor/tech-stack.md`
- `conductor/product.md`
- Architecture plan: `~/.commandcode/plans/expo-sdk-openharmony-architecture.md`
