#ifndef EXPOCONSTANTS_CONSTANTS_H
#define EXPOCONSTANTS_CONSTANTS_H

#include "ExpoModulesRegistry.h"
#include "ExpoJsiBridge.h"

namespace expo {

void RegisterExpoConstantsModule(ExpoModulesRegistry &registry);

#ifdef USE_JSI
void RegisterJsiConstantsModule(ExpoJsiBridge &bridge);
#endif

} // namespace expo

#endif
