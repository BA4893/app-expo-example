#include "ExpoConstants.h"

#include "hilog/log.h"

#include <string>

namespace expo {

namespace {

constexpr unsigned int EXPO_CONSTANTS_LOG_DOMAIN = 0x0000;
constexpr const char *EXPO_CONSTANTS_LOG_TAG = "ExpoConstants";

napi_value MakeString(napi_env env, const char *value)
{
    napi_value result = nullptr;
    napi_create_string_utf8(env, value, NAPI_AUTO_LENGTH, &result);
    return result;
}

napi_value MakeNumber(napi_env env, double value)
{
    napi_value result = nullptr;
    napi_create_double(env, value, &result);
    return result;
}

napi_value GetPlatformName(napi_env env, napi_callback_info)
{
    OH_LOG_INFO(LOG_APP, "%{public}s: getPlatformName=OpenHarmony", EXPO_CONSTANTS_LOG_TAG);
    return MakeString(env, "OpenHarmony");
}

napi_value GetSdkVersion(napi_env env, napi_callback_info)
{
    OH_LOG_INFO(LOG_APP, "%{public}s: getSdkVersion=1.0.0", EXPO_CONSTANTS_LOG_TAG);
    return MakeString(env, "1.0.0");
}

napi_value GetStatusBarHeight(napi_env env, napi_callback_info)
{
    OH_LOG_INFO(LOG_APP, "%{public}s: getStatusBarHeight=24", EXPO_CONSTANTS_LOG_TAG);
    return MakeNumber(env, 24.0);
}

napi_value GetDeviceYearClass(napi_env env, napi_callback_info)
{
    OH_LOG_INFO(LOG_APP, "%{public}s: getDeviceYearClass=2023", EXPO_CONSTANTS_LOG_TAG);
    return MakeNumber(env, 2023.0);
}

} // namespace

void RegisterExpoConstantsModule(ExpoModulesRegistry &registry)
{
    registry.Register(ModuleDefinition{
        "ExpoConstants",
        {
            {"getPlatformName", GetPlatformName},
            {"getSdkVersion", GetSdkVersion},
            {"getStatusBarHeight", GetStatusBarHeight},
            {"getDeviceYearClass", GetDeviceYearClass},
        },
    });
}

#ifdef USE_JSI

void RegisterJsiConstantsModule(ExpoJsiBridge &bridge)
{
    bridge.RegisterModule(JsiModuleDefinition{
        "ExpoConstants",
        {
            {"getPlatformName", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                return jsi::String::createFromUtf8(rt, "OpenHarmony");
            }},
            {"getSdkVersion", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                return jsi::String::createFromUtf8(rt, "1.0.0");
            }},
            {"getStatusBarHeight", [](jsi::Runtime &, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                return jsi::Value(24.0);
            }},
            {"getDeviceYearClass", [](jsi::Runtime &, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                return jsi::Value(2023.0);
            }},
        },
    });
}

#endif

} // namespace expo
