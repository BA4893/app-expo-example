#ifndef EXPOMODULESCORE_EXPOHOSTOBJECT_H
#define EXPOMODULESCORE_EXPOHOSTOBJECT_H

#include "napi/native_api.h"

namespace expo {

// Installs `globalThis.expo` and `globalThis.expo.modules`, populating the latter with
// the modules already exported on `exports`. Called once after ExportAll.
bool InstallExpoGlobal(napi_env env, napi_value exports);

} // namespace expo

#endif
