#include "ExpoModulesRegistry.h"

namespace expo {

void ExpoModulesRegistry::Register(const ModuleDefinition &definition)
{
    modules_.push_back(definition);
}

const ModuleDefinition *ExpoModulesRegistry::Find(const std::string &name) const
{
    for (const auto &mod : modules_) {
        if (name == mod.name) {
            return &mod;
        }
    }
    return nullptr;
}

namespace {

napi_value WrapMethod(napi_env env, MethodCallback callback)
{
    // Store a copy of the std::function in a native external value so it lives for the
    // lifetime of the exported object. The C++ function object owns its captured state.
    auto *holder = new MethodCallback(std::move(callback));
    napi_value fn = nullptr;
    napi_create_function(
        env,
        nullptr,
        0,
        [](napi_env callEnv, napi_callback_info info) -> napi_value {
            void *data = nullptr;
            napi_get_cb_info(callEnv, info, nullptr, nullptr, nullptr, &data);
            auto *cb = static_cast<MethodCallback *>(data);
            return (*cb)(callEnv, info);
        },
        holder,
        &fn);

    napi_add_finalizer(
        env,
        fn,
        holder,
        [](napi_env, void *data, void *) { delete static_cast<MethodCallback *>(data); },
        nullptr,
        nullptr);

    return fn;
}

} // namespace

napi_value ExpoModulesRegistry::ExportAll(napi_env env, napi_value exports)
{
    for (const auto &mod : modules_) {
        napi_value moduleObj = nullptr;
        napi_create_object(env, &moduleObj);

        for (const auto &entry : mod.methods) {
            napi_value fn = WrapMethod(env, entry.second);
            napi_set_named_property(env, moduleObj, entry.first, fn);
        }

        napi_set_named_property(env, exports, mod.name, moduleObj);
    }
    return exports;
}

} // namespace expo
