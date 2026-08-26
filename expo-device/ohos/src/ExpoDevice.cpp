#include "ExpoDevice.h"

#include "ExpoJsiBridge.h"

#include "JSValue.h"
#include "Promise.h"

#include "hilog/log.h"

#include <deviceinfo.h>
#include <cstring>

namespace expo {

namespace {

// Note: LOG_DOMAIN / LOG_TAG are macros in hilog/log.h; use distinct names here.
constexpr unsigned int EXPO_LOG_DOMAIN = 0x0000;
constexpr const char *EXPO_LOG_TAG = "ExpoDevice";

napi_value MakeString(napi_env env, const char *value)
{
    napi_value result = nullptr;
    napi_create_string_utf8(env, value == nullptr ? "" : value, NAPI_AUTO_LENGTH, &result);
    return result;
}

napi_value MakeNumber(napi_env env, double value)
{
    napi_value result = nullptr;
    napi_create_double(env, value, &result);
    return result;
}

const char *SafeDeviceType(const char *type)
{
    if (type == nullptr) {
        return "unknown";
    }
    if (std::strcmp(type, "default") == 0) {
        return "phone";
    }
    return type;
}

napi_value GetModelName(napi_env env, napi_callback_info)
{
    const char *value = OH_GetProductModel();
    OH_LOG_INFO(LOG_APP, "%{public}s: modelName=%{public}s", EXPO_LOG_TAG,
                value == nullptr ? "unknown" : value);
    return MakeString(env, value);
}

napi_value GetBrand(napi_env env, napi_callback_info)
{
    const char *value = OH_GetBrand();
    OH_LOG_INFO(LOG_APP, "%{public}s: brand=%{public}s", EXPO_LOG_TAG,
                value == nullptr ? "unknown" : value);
    return MakeString(env, value);
}

napi_value GetMarketName(napi_env env, napi_callback_info)
{
    const char *value = OH_GetMarketName();
    OH_LOG_INFO(LOG_APP, "%{public}s: marketName=%{public}s", EXPO_LOG_TAG,
                value == nullptr ? "unknown" : value);
    return MakeString(env, value);
}

napi_value GetDeviceType(napi_env env, napi_callback_info)
{
    const char *value = SafeDeviceType(OH_GetDeviceType());
    OH_LOG_INFO(LOG_APP, "%{public}s: deviceType=%{public}s", EXPO_LOG_TAG, value);
    return MakeString(env, value);
}

napi_value GetOsName(napi_env env, napi_callback_info)
{
    OH_LOG_INFO(LOG_APP, "%{public}s: osName=OpenHarmony", EXPO_LOG_TAG);
    return MakeString(env, "OpenHarmony");
}

napi_value GetOsVersion(napi_env env, napi_callback_info)
{
    const char *value = OH_GetOSFullName();
    OH_LOG_INFO(LOG_APP, "%{public}s: osVersion=%{public}s", EXPO_LOG_TAG,
                value == nullptr ? "unknown" : value);
    return MakeString(env, value);
}

napi_value GetSdkApiVersion(napi_env env, napi_callback_info)
{
    int value = OH_GetSdkApiVersion();
    OH_LOG_INFO(LOG_APP, "%{public}s: sdkApiVersion=%{public}d", EXPO_LOG_TAG, value);
    return MakeNumber(env, static_cast<double>(value));
}

} // namespace

void RegisterExpoDeviceModule(ExpoModulesRegistry &registry)
{
    registry.Register(ModuleDefinition{
        "ExpoDevice",
        {
            {"getModelName", GetModelName},
            {"getBrand", GetBrand},
            {"getMarketName", GetMarketName},
            {"getDeviceType", GetDeviceType},
            {"getOsName", GetOsName},
            {"getOsVersion", GetOsVersion},
            {"getSdkApiVersion", GetSdkApiVersion},
        },
    });
}

#ifdef USE_JSI

void RegisterJsiDeviceModule(ExpoJsiBridge &bridge)
{
    bridge.RegisterModule(JsiModuleDefinition{
        "ExpoDevice",
        {
            {"getModelName", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                const char *value = OH_GetProductModel();
                return jsi::String::createFromUtf8(rt, value == nullptr ? "unknown" : value);
            }},
            {"getBrand", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                const char *value = OH_GetBrand();
                return jsi::String::createFromUtf8(rt, value == nullptr ? "unknown" : value);
            }},
            {"getMarketName", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                const char *value = OH_GetMarketName();
                return jsi::String::createFromUtf8(rt, value == nullptr ? "unknown" : value);
            }},
            {"getDeviceType", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                const char *type = OH_GetDeviceType();
                const char *safe = (type == nullptr) ? "unknown" : (std::strcmp(type, "default") == 0 ? "phone" : type);
                return jsi::String::createFromUtf8(rt, safe);
            }},
            {"getOsName", [](jsi::Runtime &, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                return jsi::String::createFromUtf8(jsi::Runtime::getDefaultJsAllocator(), "OpenHarmony");
            }},
            {"getOsVersion", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                const char *value = OH_GetOSFullName();
                return jsi::String::createFromUtf8(rt, value == nullptr ? "unknown" : value);
            }},
            {"getSdkApiVersion", [](jsi::Runtime &, const jsi::Value &, const jsi::Value *, size_t) -> jsi::Value {
                return jsi::Value(static_cast<double>(OH_GetSdkApiVersion()));
            }},
        },
    });
}

#endif

} // namespace expo
