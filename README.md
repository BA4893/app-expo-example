# Expo SDK for OpenHarmony / HarmonyOS

Run Expo SDK modules on **OpenHarmony / HarmonyOS** devices.

This project ports the native side of Expo modules to OpenHarmony using a dual-path
bridge: a stable **N-API** path plus **JSI/Hermes** integration for HarmonyOS NEXT /
React Native for OpenHarmony (RNOH). React Native + Expo developers can build and deploy
apps for HarmonyOS with familiar `expo-*` APIs.

> **Status:** native-core SDK stable on HarmonyOS. Working modules: `expo-device`,
> `expo-file-system`, `expo-constants`. Both N-API and JSI/Hermes runtime paths are
> supported. Metro bundler + dev server integration is configured.

---

## What this is

Expo's C++ core normally runs against Facebook's `jsi::Runtime` (Hermes). OpenHarmony's
NDK exposes **N-API** (`napi_env`/`napi_value`) as the stable native interface, and
HarmonyOS NEXT also exposes **JSI/Hermes** through RNOH. This SDK provides both paths:

```
ArkTS (app) ──▶ N-API / JSI (.so) ──▶ OpenHarmony system APIs (deviceinfo, libc file I/O)
```

It is deliberately self-contained — no Huawei developer account is required for local
development and testing.

## Repository layout

```
expo-modules-core/     N-API registry + JSValue model + Promise helpers (native core)
expo-modules-core/js/  JS shims: expo-runtime, turbomodule-shim, fabric-shim, view-manager
expo-device/           expo-device native module (deviceinfo.h)
expo-file-system/      expo-file-system native module (libc POSIX)
expo-constants/        expo-constants native module
apps/test-container/   Example OpenHarmony app that consumes the modules
scripts/ohos-autolink.mjs   Resolves @expo/ohos-* deps in a consuming project
conductor/             Project orchestration (tracks, specs, learning log)
metro.config.js        Metro bundler configuration (HarmonyOS)
package.json           JS workspace manifest (React Native, Metro, Hermes)
```

## Prerequisites

| Tool | Version | Purpose |
|---|---|---|
| Node.js | >= 18 | Build scripts, ohpm/hvigor runtime |
| OpenHarmony SDK | api 20 (6.0) or HarmonyOS NEXT | Native headers + toolchain |
| `oniro-app` CLI | latest | Cross-platform build/deploy wrapper |
| Command-line tools | hvigor + ohpm | `oniro-app` resolves these |
| Hermes headers | optional | JSI/Hermes path for RNOH runtime |

On Windows, `oniro-app` expects `hvigorw.exe`/`ohpm.exe` under `~/command-line-tools/bin`
and `hdc.exe` under `~/command-line-tools/sdk/default/openharmony/toolchains`. If you have
DevEco Studio installed, those tools live under `<DevEco>/tools/{hvigor,ohpm}/bin` and
`<DevEco>/sdk/default/openharmony/toolchains` — see `conductor/learning.md` for the exact
wiring (including the `.bat`→`.exe` shim needed because `oniro-app` spawns with
`shell:false`).

## Getting started (developer)

1. **Install the toolchain.**

   ```bash
   npm i -g @oniroproject/oniro-app
   oniro-app sdk install 6.0
   oniro-app cmdtools status   # must report "Installed"
   ```

2. **Install dependencies.**

   ```bash
   npm install
   ```

3. **Build the example container.**

   ```bash
   oniro-app build apps/test-container
   ```

4. **Start an emulator and run it.**

   ```bash
   oniro-app emulator start --headless
   oniro-app emulator connect
   oniro-app app apply
   oniro-app app launch
   ```

5. **Observe the native calls in hilog.**

   ```bash
   oniro-app watch --log "ExpoDevice|ExpoFileSystem|ExpoConstants" --for 8000
   ```

   You should see device info and a file round trip, e.g.
   `ExpoFileSystem contents=hello from expo-file-system size=27`.

## Deploying an Expo app to HarmonyOS / OpenHarmony

1. **Add the native modules as workspace dependencies.**

   In your app's root `package.json`, add the Expo modules as local workspaces or
   git dependencies:

   ```json
   {
     "workspaces": [
       "node_modules/@expo/ohos-expo-device",
       "node_modules/@expo/ohos-expo-file-system",
       "node_modules/@expo/ohos-expo-constants"
     ]
   }
   ```

   Or install them with `npm`:

   ```bash
   npm install file:../expo-device file:../expo-file-system file:../expo-constants
   ```

2. **Link the native sources into your OpenHarmony module.**

   Your OpenHarmony module should have an `oh-package.json5` that references the
   native packages:

   ```json5
   {
     "dependencies": {
       "@expo/ohos-expo-device": "file:../../expo-device",
       "@expo/ohos-expo-file-system": "file:../../expo-file-system",
       "@expo/ohos-expo-constants": "file:../../expo-constants"
     }
   }
   ```

3. **Resolve and wire the native sources into your CMake build.**

   Run the autolink script to discover native sources:

   ```bash
   node scripts/ohos-autolink.mjs --project . --json
   ```

   Ensure your `CMakeLists.txt` includes the module `.cpp` files and include paths:

   ```cmake
   set(EXPOMODULESCORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../expo-modules-core/ohos)
   set(EXPODEVICE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../expo-device/ohos)
   set(EXPOFILESYSTEM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../expo-file-system/ohos)
   set(EXPOCONSTANTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../expo-constants/ohos)

   include_directories(
       ${EXPOMODULESCORE_ROOT}/include
       ${EXPODEVICE_ROOT}/include
       ${EXPOFILESYSTEM_ROOT}/include
       ${EXPOCONSTANTS_ROOT}/include
   )

   add_library(entry SHARED
       napi_init.cpp
       ${EXPOMODULESCORE_ROOT}/src/ExpoJsiBridge.cpp
       ${EXPOMODULESCORE_ROOT}/src/JsiTurboModuleFacade.cpp
       ${EXPOMODULESCORE_ROOT}/src/hermes.cpp
       ${EXPODEVICE_ROOT}/src/ExpoDevice.cpp
       ${EXPOFILESYSTEM_ROOT}/src/ExpoFileSystem.cpp
       ${EXPOCONSTANTS_ROOT}/src/ExpoConstants.cpp
   )
   ```

4. **Start Metro and launch the app.**

   ```bash
   npm run metro
   ```

   In another terminal:

   ```bash
   oniro-app app apply
   oniro-app app launch
   ```

5. **Verify native module calls from JavaScript.**

   ```typescript
   import expo from 'expo';

   async function checkDevice() {
     const model = await expo.Device.getModelNameAsync();
     console.log('Device:', model);
   }

   async function checkFileSystem() {
     await expo.FileSystem.writeFileAsync('/data/storage/el2/base/files/test.txt', 'Hello HarmonyOS');
     const contents = await expo.FileSystem.readFileAsync('/data/storage/el2/base/files/test.txt');
     console.log('File contents:', contents);
   }

   checkDevice();
   checkFileSystem();
   ```

## Consuming modules from ArkTS

If you want to call the native modules directly from ArkTS instead of JavaScript:

```typescript
import expo from 'libentry.so';

const model = expo.ExpoDevice.getModelName();
await expo.ExpoFileSystem.writeFile(filesDir + '/a.txt', 'hi');
```

## Module API surface (native)

- **`ExpoDevice`** — `getModelName()`, `getBrand()`, `getMarketName()`, `getDeviceType()`,
  `getOsName()`, `getOsVersion()`, `getSdkApiVersion()`.
- **`ExpoFileSystem`** — `writeFile(path, contents)`, `readFile(path)`, `statFile(path)`,
  `makeDirectory(path)`, `listDirectory(path)`, `deleteFile(path)` (all `Promise`-based).
- **`ExpoConstants`** — platform constants exposed as JS values.

## JS runtime shims (Track 4)

The embedded JS runtime installs on `globalThis` at N-API module init time:

- `requireNativeModule(name)` / `requireOptionalNativeModule(name)` — resolve from
  `globalThis.expo.modules` (populated by `InstallExpoGlobal`).
- `NativeModule`, `EventEmitter`, `SharedObject`, `SharedRef` — Expo class shims.
- `createTurboModule(name)` — TurboModule shim (delegates to `requireNativeModule`
  under N-API; real JSI path deferred to Track 5).
- `requireNativeComponent(name)` — Fabric shim (delegates to view-manager registry;
  real Fabric renderer deferred to Track 5).
- `requireNativeView(name)` — view-manager shim (delegates to `expo.ViewManagers`).

## Track 5 — RNOH Runtime Integration

Track 5 layers the full **React Native for OpenHarmony (RNOH)** runtime on top of the
proven native modules:

- React Native + Hermes runtime
- Metro bundler + dev server
- TurboModules + Fabric renderer + JSI bridge
- `requireNativeView` / view-manager layer

Prerequisites: HarmonyOS NEXT (API 21+), RNOH, Huawei Developer signing, physical device.

See `conductor/tracks/track-5-rnoh-integration/spec.md`.

## Verification

- **Build:** `oniro-app build apps/test-container`
- **Lint:** `oniro-app lint`
- **Runtime:** `oniro-app app apply` + `oniro-app app launch` + hilog markers.
- **Tests:** unit tests via Hypium (see `apps/test-container`).

## License

MIT.
