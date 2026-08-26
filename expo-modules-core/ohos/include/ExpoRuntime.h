#ifndef EXPOMODULESCORE_EXPORUNTIME_H
#define EXPOMODULESCORE_EXPORUNTIME_H

#include "napi/native_api.h"

namespace expo {

// Runs the embedded Expo JS shim (requireNativeModule, NativeModule, EventEmitter,
// SharedObject, SharedRef, TurboModule, Fabric, requireNativeView) via napi_run_script,
// installing them on globalThis. Must be called after InstallExpoGlobal so
// globalThis.expo.modules is populated.
bool InstallExpoRuntime(napi_env env);

// Verification helper: runs requireNativeModule('ExpoDevice').getModelName() and logs the
// result to hilog, proving the JS -> N-API -> native round trip.
bool VerifyExpoRuntime(napi_env env);

} // namespace expo

#endif
