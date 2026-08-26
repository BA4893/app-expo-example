#include "ExpoHostObject.h"

#include "hilog/log.h"

namespace expo {

namespace {

constexpr unsigned int EXPO_HOST_DOMAIN = 0x0000;
constexpr const char *EXPO_HOST_TAG = "ExpoHostObject";

bool GetGlobal(napi_env env, const char *name, napi_value *out)
{
    napi_value global = nullptr;
    if (napi_get_global(env, &global) != napi_ok) {
        return false;
    }
    return napi_get_named_property(env, global, name, out) == napi_ok;
}

bool SetGlobal(napi_env env, const char *name, napi_value value)
{
    napi_value global = nullptr;
    if (napi_get_global(env, &global) != napi_ok) {
        return false;
    }
    return napi_set_named_property(env, global, name, value) == napi_ok;
}

// Reads every enumerable property off `exports` and copies it onto `modules`.
bool CopyExportsToModules(napi_env env, napi_value exports, napi_value modules)
{
    napi_value keys = nullptr;
    if (napi_get_property_names(env, exports, &keys) != napi_ok) {
        return false;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, keys, &length) != napi_ok) {
        return false;
    }

    for (uint32_t i = 0; i < length; i++) {
        napi_value key = nullptr;
        napi_get_element(env, keys, i, &key);

        napi_value value = nullptr;
        napi_get_property(env, exports, key, &value);

        napi_set_property(env, modules, key, value);
    }
    return true;
}

} // namespace

bool InstallExpoGlobal(napi_env env, napi_value exports)
{
    napi_value existingExpo = nullptr;
    bool hasExpo = GetGlobal(env, "expo", &existingExpo) && existingExpo != nullptr;

    napi_value expoObj = nullptr;
    napi_value modules = nullptr;

    if (hasExpo) {
        // Reuse an existing global.expo (from a prior native install) to stay idempotent.
        expoObj = existingExpo;
        napi_get_named_property(env, expoObj, "modules", &modules);
        if (modules == nullptr) {
            napi_create_object(env, &modules);
            napi_set_named_property(env, expoObj, "modules", modules);
        }
    } else {
        napi_create_object(env, &expoObj);
        napi_create_object(env, &modules);
        napi_set_named_property(env, expoObj, "modules", modules);
        SetGlobal(env, "expo", expoObj);
    }

    bool ok = CopyExportsToModules(env, exports, modules);
    if (ok) {
        OH_LOG_INFO(LOG_APP, "%{public}s: installed globalThis.expo.modules",
                    EXPO_HOST_TAG);
    } else {
        OH_LOG_ERROR(LOG_APP, "%{public}s: failed to install globalThis.expo.modules",
                     EXPO_HOST_TAG);
    }
    return ok;
}

} // namespace expo
