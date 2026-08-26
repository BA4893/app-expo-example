# Workflow

- Prefers routing planning and architecture work to the @conductor-dev agent/skill (e.g., Conductor-driven track/spec planning). Confidence: 0.8
- Prefers using the @ohos-app-dev agent/skill for the inner development loop (compile, debug C++/ArkTS lifecycles, check hilog logs, test against a connected device/emulator). Confidence: 0.8
- Prefers terse, high-level directives (e.g., "please execute the next step") and expects the agent to autonomously carry the next track/step end to end — plan, code, build, verify, and document — without granular step-by-step hand-holding. Confidence: 0.7
- Prefers empirically investigating the actual toolchain/API surface (reading SDK headers, inspecting installed ohpm/HAR packages, checking tool tasks/config) before writing specs/plans or code. Confidence: 0.8
- Prefers consulting upstream/open-source reference implementations (e.g., reading expo-modules-core source) to nail down exact contracts/interfaces before implementing a compatibility layer. Confidence: 0.7
- Prefers pragmatic minimal implementations that satisfy a contract without heavyweight dependencies — e.g., a hand-written requireNativeModule/NativeModule shim plus a native globalThis.expo host object instead of Metro/Hermes/JSI. Confidence: 0.6
- Consistently accepts the agent's recommended option when presented with choices (answered every planning/decision question with the "Recommended" option, including the OpenHarmony+N-API pivot); prefers a clear single recommendation with brief alternatives over open-ended decisions. Confidence: 0.75
- Accepts scope deferrals and direction pivots when they are recorded transparently in the track spec/learning log (e.g., RNOH integration deferred out of Track 0, N-API-only pivot documented); expects deferred scope to be picked up by a subsequent track. Confidence: 0.7
