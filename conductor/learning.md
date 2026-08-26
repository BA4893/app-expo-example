# Learning Log

## 2026-08-23 — Toolchain discovery (Track 0)

- **Finding:** `oniro-app` (v0.8.1) resolves build tools from `~/command-line-tools`:
  - `ohpm` → `<cmdToolsPath>/bin/ohpm.*`
  - `hvigorw` → project-local `hvigorw`/`hvigor/node_modules`, else `<cmdToolsPath>/bin/hvigorw.*`
  - `hdc` → `<cmdToolsPath>/sdk/default/openharmony/toolchains/hdc.exe`
- **Symptom:** `oniro-app build`/`devices` failed with `ENOENT`; `cmdtools status` reported "Not installed".
- **Root cause:** The `~/command-line-tools` layout is incomplete (only a stray `tools/` prefix), so none of the expected `bin/` or `sdk/` paths resolve.
- **Resolution:** A full DevEco Studio 6.1.1 install exists at `D:\deveco2026\DevEco Studio` with all three tools:
  - `hvigorw` (6.24.2) at `tools/hvigor/bin/hvigorw.bat`
  - `ohpm` (6.1.2) at `tools/ohpm/bin/ohpm.bat`
  - `hdc` at `sdk/default/openharmony/toolchains/hdc.exe`
  Wire these into the layout `oniro-app` expects (env override `ONIRO_CMD_TOOLS_PATH` or a corrected `~/command-line-tools` tree).
- **Note:** SDK 6.0 (api 20) is installed at `C:\Users\Benja\setup-ohos-sdk\windows\20`.
- **Note:** Node is v24.11.1; `npm` must be invoked as `npm.cmd` because the `.ps1` shim is blocked by execution policy.

## 2026-08-24 — Build toolchain relay shim (Track 0, resolved)

- **Symptom:** after wiring the DevEco tools into `~/command-line-tools`, `oniro-app build`
  still failed: `Failed to spawn ...\ohpm.exe ... ENOENT` (missing file). The DevEco tools
  ship only as Node-based `.bat`/`.js` wrappers, not native `.exe`.
- **Root cause (Windows):** `oniro-app` spawns `hvigorw`/`ohpm` with `shell: false`.
  Node 24 cannot execute a `.bat`/`.cmd` file that way (`EINVAL`, confirmed by test).
- **Resolution:** built a tiny Rust relay shim (`oniro-shim`) that dispatches to the real
  Node entry points:
  - `hvigorw.exe` → `node.exe <deveco>/tools/hvigor/bin/hvigorw.js`
  - `ohpm.exe` → `node.exe <deveco>/tools/ohpm/bin/pm-cli.js`
  Deployed as `~/command-line-tools/bin/{hvigorw,ohpm}.exe`. Copied the DevEco `hdc`
  toolchain tree into `~/command-line-tools/sdk/default/openharmony/toolchains/`.
- **Result:** `oniro-app cmdtools status` → `Installed (6.1.2.268)`; `oniro-app build`
  succeeds (native CMake + ArkTS → `entry-default-unsigned.hap`).
- **Build success baseline:** `hvigor` 6.24.2 + `ohpm` 6.1.2 + SDK api 20.
- **Note:** no device/emulator connected (`oniro-app devices` → `[]`), so deploy/launch
  and hilog observation are still pending a device.

## 2026-08-24 — Inner-loop verification (Track 0, resolved)

- **Emulator:** `oniro-app emulator start --headless` + `oniro-app emulator connect`
  exposes a `127.0.0.1:55555` TCP target; `oniro-app devices` sees it.
- **Signing:** `oniro-app sign` generated `signingConfigs` and produced a signed HAP
  (`entry-default-signed.hap`); the unsigned HAP alone was not installable.
- **Deploy/launch:** `oniro-app app apply` (replace) + `oniro-app app launch` start the
  container; the app renders "Hello World" on the emulator.
- **hilog:** confirmed the full path in `A00000/testTag`:
  `Ability onCreate` → `onWindowStageCreate` → `onForeground` → `Succeeded in loading
  the content`, plus a tap-triggered NAPI call `Test NAPI 2 + 3 = 5` (ArkTS → C++
  boundary).
- **Raw hilog fallback:** `oniro-app watch --log` returned empty on this emulator; used
  `<hdc> shell hilog -x | grep` directly for reliable verification.

## 2026-08-24 — Track 1 direction: OpenHarmony + own N-API bridge (decision)

- **Finding:** RNOH (`linhandev/ohos_react_native`, branch `0.77.1-rc.1-ohos`) targets
  `runtimeOS: "HarmonyOS"`, `compatibleSdkVersion: "5.0.1(13)"`, and requires **HarmonyOS
  NEXT** + a **Huawei developer account** for signing. It publishes no downloadable `.har`
  (must build from source). Our validated toolchain is `oniro-app` + OpenHarmony api 20.
- **Decision:** stay on OpenHarmony api 20 and implement Expo's C++ bridge against **N-API
  directly**; do not pull in RNOH/Hermes. RNOH integration is deferred.
- **Critical constraint:** OpenHarmony's native sysroot has `js_native_api.h` / `node_api.h`
  but **no `jsi`/Hermes headers**. Expo's C++ core is written against `facebook::jsi`. So the
  bridge cannot reuse JSI — it must be a clean N-API backend (`napi_env`, `napi_value`),
  which is exactly the plan's §1.4 "new `ohos/` platform backend".
- **Device surface available:** `deviceinfo.h` (`libdeviceinfo_ndk.z.so`) exposes
  `OH_GetProductModel`, `OH_GetBrand`, `OH_GetMarketName`, `OH_GetDeviceType`,
  `OH_GetOSFullName`, `OH_GetSdkApiVersion` — enough for `expo-device`.

## 2026-08-24 — Track 1 build/compile lessons (resolved)

- **`napi_is_function` does not exist** in OpenHarmony N-API; use `napi_typeof` returning
  `napi_function`, or just treat non-arraybuffer `napi_object` as `Object`/`Function`.
- **`hilog/log.h` defines `LOG_DOMAIN` and `LOG_TAG` macros.** Local consts with those
  names collide and produce confusing `%s`→`%ld` format errors. Use distinct names
  (`EXPO_LOG_DOMAIN`, `EXPO_LOG_TAG`).
- **`deviceinfo.h` must be explicitly included** for `OH_GetProductModel` etc.
- **CMake relative paths:** sources at repo root required one more `..` than expected from
  `entry/src/main/cpp` (`../../../../../../expo-modules-core/ohos`).
- **CMake target name must match `target_link_libraries`** (renamed `expo` → `entry`);
  a stale target in `target_link_libraries` fails with "not built by this project".

## 2026-08-24 — Track 1 verified round trip (resolved)

- Built `libentry.so` with the N-API bridge (`ExpoModulesRegistry`, `JSValue`, `Promise`)
  plus `expo-device` (`deviceinfo.h`), linked against `libace_napi.z.so`,
  `libdeviceinfo_ndk.z.so`, `libhilog_ndk.z.so`.
- On emulator, `expo.ExpoDevice.*` returned real values from the C API:
  `model=ohos`, `brand=default`, `os=OpenHarmony OpenHarmony-6.0.0.47(Beta2)`.
- Emulator quirk: QEMU boots slowly and can report "Offline" until `hdc` is killed and
  reconnected (`oniro-app emulator connect`). Two back-to-back starts can leave a stale
  `hdc` server; restart it and reconnect.

## 2026-08-24 — Track 2 file-system lessons (resolved)

- **`@ohos.file.fs` is ArkTS-only.** For native N-API code, the C-level equivalent is libc
  POSIX (`open`/`read`/`write`/`stat`/`mkdir`/`opendir`/`readdir`/`unlink`/`rename`), all
  present in the sysroot and linked via libc. `oh_fileio.h` only has `GetFileLocation` —
  not general file ops.
- **Sandbox path comes from ArkTS.** `this.getUIContext().getHostContext()` returns the
  `UIAbilityContext`; its `.filesDir` is the writable app sandbox. `getContext(this)` is
  deprecated at api 20.
- **Promise primitives:** use `napi_create_promise` + `napi_resolve_deferred` /
  `napi_reject_deferred` (not the `Promise.resolve/reject` JS-global trick) for clean async
  results from native modules.
- **Verified:** `expo-file-system` write/read/stat/mkdir/list/delete round trip returned
  `contents=hello from expo-file-system size=27 dirCount=5` through promise-resolved N-API.

## 2026-08-24 — Track 3 packaging lessons (resolved)

- **A library SDK is not a root app project.** A root `build-profile.json5`/`hvigorfile.ts`
  implies a root *app* (needs `AppScope/app.json5`). A monorepo of HAR packages should
  instead be consumed by a real app module (e.g. `apps/test-container`) via `file:` deps.
- **ohpm local `file:` deps** resolve against the *module* `oh-package.json5`, not a
  workspace root; the SDK packages carry publishable metadata but no registry is available
  locally, so consumption is via `file:` paths.
- **`codelinter` is a DevEco plugin, not a standalone binary** — `oniro-app lint` fails with
  `ENOENT` and cannot be wired via a simple shim (no clean CLI entry point). The ArkTS
  compiler already enforces strict mode during `oniro-app build`, so **build is the primary
  lint gate** on this toolchain.
- **Autolinking:** `scripts/ohos-autolink.mjs` resolves `@expo/ohos-*` deps to local package
  paths (verified against the root workspace).
