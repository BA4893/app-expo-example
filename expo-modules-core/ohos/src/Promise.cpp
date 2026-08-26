#include "Promise.h"

namespace expo {

namespace {

napi_value GetPromiseConstructor(napi_env env)
{
    napi_value global = nullptr;
    napi_get_global(env, &global);

    napi_value promiseCtor = nullptr;
    napi_get_named_property(env, global, "Promise", &promiseCtor);
    return promiseCtor;
}

} // namespace

bool CreateResolvedPromise(napi_env env, napi_value resolution, napi_value &outPromise)
{
    napi_value ctor = GetPromiseConstructor(env);
    napi_value resolveFn = nullptr;
    napi_get_named_property(env, ctor, "resolve", &resolveFn);

    napi_value arg = resolution;
    return napi_call_function(env, ctor, resolveFn, 1, &arg, &outPromise) == napi_ok;
}

bool CreateRejectedPromise(napi_env env, napi_value reason, napi_value &outPromise)
{
    napi_value ctor = GetPromiseConstructor(env);
    napi_value rejectFn = nullptr;
    napi_get_named_property(env, ctor, "reject", &rejectFn);

    napi_value arg = reason;
    return napi_call_function(env, ctor, rejectFn, 1, &arg, &outPromise) == napi_ok;
}

napi_value PromiseResolved(napi_env env, napi_value resolution)
{
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    if (napi_create_promise(env, &deferred, &promise) != napi_ok) {
        return nullptr;
    }
    napi_resolve_deferred(env, deferred, resolution);
    return promise;
}

napi_value PromiseRejected(napi_env env, napi_value reason)
{
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    if (napi_create_promise(env, &deferred, &promise) != napi_ok) {
        return nullptr;
    }
    napi_reject_deferred(env, deferred, reason);
    return promise;
}

} // namespace expo
