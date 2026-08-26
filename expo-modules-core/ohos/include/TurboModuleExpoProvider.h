#ifndef EXPOMODULESCORE_TURBOMODULEEXPOPROVIDER_H
#define EXPOMODULESCORE_TURBOMODULEEXPOPROVIDER_H

#include "napi/native_api.h"

#include <memory>
#include <string>

namespace expo {

#ifdef USE_JSI

// Returns a TurboModuleFactory that creates Expo modules as JSI HostObjects
// registered with the Hermes runtime. Used by the RNOH TurboModule resolver.
std::shared_ptr<facebook::react::TurboModuleFactory> GetExpoTurboModuleFactory();

#endif

} // namespace expo

#endif
