#ifndef EXPODEVICE_DEVICE_H
#define EXPODEVICE_DEVICE_H

#include "ExpoModulesRegistry.h"
#include "ExpoJsiBridge.h"

namespace expo {

void RegisterExpoDeviceModule(ExpoModulesRegistry &registry);
void RegisterJsiDeviceModule(ExpoJsiBridge &bridge);

} // namespace expo

#endif
