#include "TurboModuleExpoProvider.h"

#include "jsi/jsi.h"
#include "hermes/hermes.h"
#include "react/ReactCommon/TurboModule.h"

#include "ExpoJsiBridge.h"

#include "hilog/log.h"

#include <memory>
#include <string>

namespace expo {

namespace {

constexpr unsigned int EXPO_TMP_DOMAIN = 0x0000;
constexpr const char *EXPO_TMP_TAG = "ExpoTurboModuleProvider";

#ifdef USE_JSI

class ExpoTurboModule : public facebook::react::TurboModule {
public:
    explicit ExpoTurboModule(std::shared_ptr<facebook::jsi::Object> jsiModule)
        : jsiModule_(std::move(jsiModule)) {}

    std::string getName() override {
        return "ExpoTurboModule";
    }

private:
    std::shared_ptr<facebook::jsi::Object> jsiModule_;
};

class ExpoTurboModuleFactory : public facebook::react::TurboModuleFactory {
public:
    std::shared_ptr<facebook::react::TurboModule> createModule(
        facebook::jsi::Runtime &runtime,
        const std::string &name) override {
        ExpoJsiBridge &bridge = ExpoJsiBridge::Instance();
        const JsiModuleDefinition *def = bridge.Find(name);
        if (!def) {
            OH_LOG_ERROR(LOG_APP, "%{public}s: module '%{public}s' not found in JSI bridge", EXPO_TMP_TAG, name.c_str());
            return nullptr;
        }

        std::vector<JsiMethodDefinition> jsiMethods;
        for (const auto &entry : def->methods) {
            jsiMethods.push_back(entry);
        }

        auto hostObject = std::make_shared<ExpoJsiHostObject>(jsiMethods);
        auto moduleObj = jsi::Object::createFromHostObject(runtime, hostObject);

        OH_LOG_INFO(LOG_APP, "%{public}s: created TurboModule '%{public}s'", EXPO_TMP_TAG, name.c_str());
        return std::make_shared<ExpoTurboModule>(moduleObj);
    }
};

#endif

} // namespace

#ifdef USE_JSI

std::shared_ptr<facebook::react::TurboModuleFactory> GetExpoTurboModuleFactory() {
    static std::shared_ptr<ExpoTurboModuleFactory> factory =
        std::make_shared<ExpoTurboModuleFactory>();
    return factory;
}

#endif

} // namespace expo
