#ifndef EXPOMODULESCORE_FABRICVIEWMANAGERFACADE_H
#define EXPOMODULESCORE_FABRICVIEWMANAGERFACADE_H

#include "napi/native_api.h"

namespace expo {

// Fabric view-manager integration facade.
//
// On OpenHarmony api 20 (no RNOH), this is a no-op stub that returns false.
// When built against RNOH / Fabric (HarmonyOS NEXT), the implementation registers
// native view managers with the Fabric renderer.
//
// Usage:
//   bool ok = RegisterFabricViewManagers(env);
//   if (!ok) { /* fall back to N-API / ArkTS UI */ }

bool RegisterFabricViewManagers(napi_env env);

} // namespace expo

#endif
