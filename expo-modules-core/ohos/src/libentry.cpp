#include "ExpoModulesRegistry.h"

#include "ExpoJsiBridge.h"
#include "ExpoHostObject.h"
#include "ExpoRuntime.h"

#include "jsi/jsi.h"
#include "hermes/hermes.h"

#include "hilog/log.h"

#include <string>
#include <vector>

namespace expo {

namespace {

constexpr unsigned int EXPO_ENTRY_DOMAIN = 0x0000;
constexpr const char *EXPO_ENTRY_TAG = "ExpoEntry";

// Forward declarations for module registration functions (each module provides one).
void RegisterExpoDeviceModule(ExpoModulesRegistry &registry);
void RegisterExpoFileSystemModule(ExpoModulesRegistry &registry);
void RegisterExpoConstantsModule(ExpoModulesRegistry &registry);

void RegisterJsiDeviceModule(ExpoJsiBridge &bridge);
void RegisterJsiFileSystemModule(ExpoJsiBridge &bridge);
void RegisterJsiConstantsModule(ExpoJsiBridge &bridge);

} // namespace

// N-API module init — called by the HarmonyOS N-API loader when the .so is loaded.
napi_value Init(napi_env env, napi_value exports) {
    ExpoModulesRegistry registry;
    RegisterExpoDeviceModule(registry);
    RegisterExpoFileSystemModule(registry);
    RegisterExpoConstantsModule(registry);

    napi_value allModules = nullptr;
    napi_create_object(env, &allModules);
    registry.ExportAll(env, allModules);

    InstallExpoGlobal(env, allModules);
    InstallExpoRuntime(env);

    OH_LOG_INFO(LOG_APP, "%{public}s: N-API exports installed", EXPO_ENTRY_TAG);

    return exports;
}

#ifdef USE_JSI

// JSI bridge init — called from InstallExpoTurboModuleShim when a Hermes runtime
// is available (HarmonyOS NEXT / RNOH).
bool InstallExpoJsiBridge(jsi::Runtime &runtime) {
    ExpoJsiBridge &bridge = ExpoJsiBridge::Instance();

    RegisterJsiDeviceModule(bridge);
    RegisterJsiFileSystemModule(bridge);
    RegisterJsiConstantsModule(bridge);

    bool ok = bridge.InstallIntoRuntime(runtime);
    if (ok) {
        OH_LOG_INFO(LOG_APP, "%{public}s: JSI bridge installed into Hermes runtime", EXPO_ENTRY_TAG);
    }

    return ok;
}

#endif

} // namespace expo

NAPI_MODULE(expoModulesCore, Init)
