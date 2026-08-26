#include "JsiTurboModuleFacade.h"

#include "ExpoJsiBridge.h"

#include "ExpoDevice.h"
#include "ExpoFileSystem.h"
#include "ExpoConstants.h"

#include "jsi/jsi.h"
#include "hermes/hermes.h"

#include "hilog/log.h"

#include <string>

namespace expo {

namespace {

constexpr unsigned int EXPO_TM_DOMAIN = 0x0000;
constexpr const char *EXPO_TM_TAG = "ExpoTurboModule";

} // namespace

bool InstallExpoTurboModuleShimNapi(napi_env env)
{
    OH_LOG_INFO(LOG_APP, "%{public}s: TurboModule shim installed (N-API fallback)", EXPO_TM_TAG);
    return true;
}

#ifdef USE_JSI

bool InstallExpoTurboModuleShim(jsi::Runtime &runtime) {
    OH_LOG_INFO(LOG_APP, "%{public}s: installing Expo TurboModules via JSI on runtime %p", EXPO_TM_TAG, (void *)&runtime);

    ExpoJsiBridge &bridge = ExpoJsiBridge::Instance();

    RegisterJsiDeviceModule(bridge);
    RegisterJsiFileSystemModule(bridge);
    RegisterJsiConstantsModule(bridge);

    bool ok = bridge.InstallIntoRuntime(runtime);
    if (!ok) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: failed to install JSI bridge", EXPO_TM_TAG);
        return false;
    }

    OH_LOG_INFO(LOG_APP, "%{public}s: Expo TurboModules installed", EXPO_TM_TAG);
    return true;
}

#endif

} // namespace expo
