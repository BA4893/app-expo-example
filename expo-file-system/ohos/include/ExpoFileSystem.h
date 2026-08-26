#ifndef EXPOFILESYSTEM_FILESYSTEM_H
#define EXPOFILESYSTEM_FILESYSTEM_H

#include "ExpoModulesRegistry.h"
#include "ExpoJsiBridge.h"

namespace expo {

void RegisterExpoFileSystemModule(ExpoModulesRegistry &registry);
void RegisterJsiFileSystemModule(ExpoJsiBridge &bridge);

} // namespace expo

#endif
