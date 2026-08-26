#include "JSValue.h"

namespace expo {

bool ReadJSValue(napi_env env, napi_value value, JSValue &out)
{
    napi_valuetype type;
    if (napi_typeof(env, value, &type) != napi_ok) {
        return false;
    }

    switch (type) {
    case napi_undefined:
        out.type = JSValueType::Undefined;
        return true;
    case napi_null:
        out.type = JSValueType::Null;
        return true;
    case napi_boolean: {
        bool b = false;
        if (napi_get_value_bool(env, value, &b) != napi_ok) {
            return false;
        }
        out.type = JSValueType::Boolean;
        out.boolean = b;
        return true;
    }
    case napi_number: {
        double n = 0.0;
        if (napi_get_value_double(env, value, &n) != napi_ok) {
            return false;
        }
        out.type = JSValueType::Number;
        out.number = n;
        return true;
    }
    case napi_string: {
        size_t len = 0;
        if (napi_get_value_string_utf8(env, value, nullptr, 0, &len) != napi_ok) {
            return false;
        }
        std::string s(len, '\0');
        if (napi_get_value_string_utf8(env, value, &s[0], len + 1, &len) != napi_ok) {
            return false;
        }
        out.type = JSValueType::String;
        out.string = std::move(s);
        return true;
    }
    case napi_object: {
        bool isArrayBuffer = false;
        if (napi_is_arraybuffer(env, value, &isArrayBuffer) == napi_ok && isArrayBuffer) {
            void *data = nullptr;
            size_t len = 0;
            if (napi_get_arraybuffer_info(env, value, &data, &len) != napi_ok) {
                return false;
            }
            out.type = JSValueType::ArrayBuffer;
            out.data = data;
            out.length = len;
            return true;
        }

        // napi_typeof returns napi_function for callables; no napi_is_function exists.
        out.type = JSValueType::Object;
        return napi_create_reference(env, value, 1, &out.ref) == napi_ok;
    }
    case napi_function:
        out.type = JSValueType::Function;
        return napi_create_reference(env, value, 1, &out.ref) == napi_ok;
    case napi_symbol:
    case napi_external:
    case napi_bigint:
    default:
        return false;
    }
}

void ReleaseJSValue(napi_env env, JSValue &value)
{
    if (value.ref != nullptr) {
        napi_delete_reference(env, value.ref);
        value.ref = nullptr;
    }
}

} // namespace expo
