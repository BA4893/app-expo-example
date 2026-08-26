#include "ExpoModulesRegistry.h"
#include "ExpoDevice.h"
#include "ExpoFileSystem.h"
#include "ExpoHostObject.h"
#include "ExpoRuntime.h"
#include "JsiTurboModuleFacade.h"
#include "FabricViewManagerFacade.h"

#include "napi/native_api.h"

using namespace expo;

static ExpoModulesRegistry &GetRegistry()
{
    static ExpoModulesRegistry registry;
    return registry;
}

static napi_value Init(napi_env env, napi_value exports)
{
    ExpoModulesRegistry &registry = GetRegistry();
    RegisterExpoDeviceModule(registry);
    RegisterExpoFileSystemModule(registry);
    napi_value all = registry.ExportAll(env, exports);
    InstallExpoGlobal(env, all);
    InstallExpoRuntime(env);
    InstallExpoTurboModuleShimNapi(env);
    RegisterFabricViewManagers(env);
    VerifyExpoRuntime(env);
    return all;
}

EXTERN_C_START
static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "expo",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterExpoModules(void)
{
    napi_module_register(&demoModule);
}
EXTERN_C_END
