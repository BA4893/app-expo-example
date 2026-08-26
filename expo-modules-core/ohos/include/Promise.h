#ifndef EXPOMODULESCORE_PROMISE_H
#define EXPOMODULESCORE_PROMISE_H

#include "napi/native_api.h"

namespace expo {

// Resolves a fresh JS promise with a napi_value.
bool CreateResolvedPromise(napi_env env, napi_value resolution, napi_value &outPromise);

// Rejects a fresh JS promise with a napi_value (an Error is typical).
bool CreateRejectedPromise(napi_env env, napi_value reason, napi_value &outPromise);

// Creates a promise via napi_create_promise and immediately resolves it.
napi_value PromiseResolved(napi_env env, napi_value resolution);

// Creates a promise via napi_create_promise and immediately rejects it.
napi_value PromiseRejected(napi_env env, napi_value reason);

} // namespace expo

#endif
