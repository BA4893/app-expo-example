#include "ExpoJsiBridge.h"

#include "jsi/jsi.h"
#include "hermes/hermes.h"

#include "hilog/log.h"

#include <string>

namespace expo {

namespace {

constexpr unsigned int EXPO_JSI_DOMAIN = 0x0000;
constexpr const char *EXPO_JSI_TAG = "ExpoJsiBridge";

jsi::Value NapiValueToJsiValue(jsi::Runtime &runtime, napi_env env, napi_value value) {
    napi_valuetype type;
    if (napi_typeof(env, value, &type) != napi_ok) {
        return jsi::Value::undefined();
    }

    switch (type) {
    case napi_undefined:
        return jsi::Value::undefined();
    case napi_null:
        return jsi::Value::null();
    case napi_boolean: {
        bool b = false;
        napi_get_value_bool(env, value, &b);
        return jsi::Value(b);
    }
    case napi_number: {
        double n = 0.0;
        napi_get_value_double(env, value, &n);
        return jsi::Value(n);
    }
    case napi_string: {
        size_t len = 0;
        napi_get_value_string_utf8(env, value, nullptr, 0, &len);
        std::string s(len, '\0');
        napi_get_value_string_utf8(env, value, &s[0], len + 1, &len);
        return jsi::String::createFromUtf8(runtime, s.c_str());
    }
    case napi_object:
    case napi_function:
    case napi_symbol:
    case napi_external:
    case napi_bigint:
    default:
        return jsi::Value::undefined();
    }
}

napi_value JsiValueToNapiValue(napi_env env, const jsi::Value &value) {
    if (value.isUndefined() || value.isNull()) {
        napi_value result = nullptr;
        napi_get_null(env, &result);
        return result;
    }
    if (value.isBool()) {
        napi_value result = nullptr;
        napi_get_boolean(env, value.getBool(), &result);
        return result;
    }
    if (value.isNumber()) {
        napi_value result = nullptr;
        napi_create_double(env, value.getNumber(), &result);
        return result;
    }
    if (value.isString()) {
        std::string str = value.getString(jsi::Runtime::getDefaultJsAllocator())->utf8(
            jsi::Runtime::getDefaultJsAllocator());
        napi_value result = nullptr;
        napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &result);
        return result;
    }
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

} // namespace

ExpoJsiBridge &ExpoJsiBridge::Instance() {
    static ExpoJsiBridge instance;
    return instance;
}

void ExpoJsiBridge::RegisterModule(const JsiModuleDefinition &definition) {
    modules_.push_back(definition);
    OH_LOG_INFO(
        LOG_APP,
        "%{public}s: registered JSI module '%{public}s'",
        EXPO_JSI_TAG,
        definition.name);
}

bool ExpoJsiBridge::InstallIntoRuntime(jsi::Runtime &runtime) {
    for (const auto &mod : modules_) {
        std::vector<JsiMethodDefinition> jsiMethods;
        for (const auto &entry : mod.methods) {
            jsiMethods.push_back(entry);
        }

        auto hostObject = std::make_shared<ExpoJsiHostObject>(jsiMethods);
        auto moduleObj = jsi::Object::createFromHostObject(runtime, hostObject);

        runtime.global()->setProperty(runtime, mod.name, jsi::Value(moduleObj));

        OH_LOG_INFO(
            LOG_APP,
            "%{public}s: installed JSI module '%{public}s' on global",
            EXPO_JSI_TAG,
            mod.name);
    }

    return true;
}

const JsiModuleDefinition *ExpoJsiBridge::Find(const std::string &name) const {
    for (const auto &mod : modules_) {
        if (name == mod.name) {
            return &mod;
        }
    }
    return nullptr;
}

} // namespace expo
