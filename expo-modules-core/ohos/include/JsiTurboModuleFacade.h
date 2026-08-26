#ifndef EXPOMODULESCORE_JSITURBOMODULEFACADE_H
#define EXPOMODULESCORE_JSITURBOMODULEFACADE_H

#include "napi/native_api.h"
#include "jsi/jsi.h"

namespace expo {

// N-API fallback path: installs TurboModule shim without JSI.
// Always available regardless of Hermes / RNOH presence.
bool InstallExpoTurboModuleShimNapi(napi_env env);

#ifdef USE_JSI

// JSI path: installs Expo modules as JSI HostObjects on the Hermes runtime
// provided by RNOH. Call this from the RNOH TurboModule resolver context.
bool InstallExpoTurboModuleShim(jsi::Runtime &runtime);

#else

// When Hermes is not available, the N-API shim is the only path.
inline bool InstallExpoTurboModuleShim(napi_env env) {
    return InstallExpoTurboModuleShimNapi(env);
}

#endif

} // namespace expo

#endif
