#ifndef EXPOMODULESCORE_TURBOMODULE_H
#define EXPOMODULESCORE_TURBOMODULE_H

#include <jsi/jsi.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace facebook {
namespace react {

class TurboModule {
public:
  virtual ~TurboModule() = default;
  virtual std::unordered_map<std::string, jsi::Value> getConstants() = 0;
};

class TurboModuleBinding {
public:
  virtual ~TurboModuleBinding() = default;
  virtual std::shared_ptr<TurboModule> createTurboModule(
      jsi::Runtime& runtime,
      const std::shared_ptr<jsi::Object>& module) = 0;
};

class TurboModuleProvider {
public:
  virtual ~TurboModuleProvider() = default;
  virtual std::shared_ptr<TurboModule> getTurboModule(
      const std::string& moduleName) = 0;
};

class TurboModuleManager {
public:
  virtual ~TurboModuleManager() = default;
  virtual std::shared_ptr<TurboModule> getTurboModule(const std::string& moduleName) = 0;
};

} // namespace react
} // namespace facebook

#endif
