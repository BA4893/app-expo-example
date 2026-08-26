#include "FabricViewManagerFacade.h"

#include "jsi/jsi.h"
#include "hermes/hermes.h"
#include "react/ReactCommon/FabricViewManager.h"

#include "hilog/log.h"

#include <string>

namespace expo {

namespace {

constexpr unsigned int EXPO_FABRIC_DOMAIN = 0x0000;
constexpr const char *EXPO_FABRIC_TAG = "ExpoFabricViewManager";

#ifdef USE_JSI

class ExpoViewManager : public facebook::react::ViewManager {
public:
    std::string name_;

    explicit ExpoViewManager(std::string name) : name_(std::move(name)) {}

    std::string getName() const override {
        return name_;
    }

    std::shared_ptr<facebook::jsi::Object> createView(
        facebook::jsi::Runtime& runtime,
        const std::shared_ptr<facebook::jsi::Object>& props) override {
        auto viewObj = runtime.createObject();
        auto nameVal = jsi::String::createFromUtf8(runtime, name_.c_str());
        viewObj->setProperty(runtime, "name", jsi::Value(nameVal));
        if (props && props->hasProperty(runtime, "props")) {
            auto propsValue = props->getProperty(runtime, "props");
            if (propsValue.isObject()) {
                auto propsObj = propsValue.getObject(runtime);
                auto names = propsObj->getPropertyNames(runtime);
                for (const auto& propName : names) {
                    std::string nameStr = propName->utf8(runtime);
                    viewObj->setProperty(runtime, nameStr.c_str(), propsObj->getProperty(runtime, propName));
                }
            }
        }
        return viewObj;
    }
};

bool RegisterFabricViewManagerImpl(facebook::jsi::Runtime& runtime, const std::string& viewManagerName) {
    auto viewManager = std::make_shared<ExpoViewManager>(viewManagerName);
    facebook::react::FabricViewManagerFacade::getInstance().registerViewManager(runtime, viewManager);

    OH_LOG_INFO(LOG_APP, "%{public}s: registered Fabric view manager '%{public}s'", EXPO_FABRIC_TAG, viewManagerName.c_str());
    return true;
}

#endif

} // namespace

bool RegisterFabricViewManagers(napi_env env) {
#ifdef USE_JSI
    OH_LOG_INFO(LOG_APP, "%{public}s: registering Fabric view managers via JSI", EXPO_FABRIC_TAG);

    auto hermesRuntime = facebook::hermes::HermesRuntime::create();
    if (!hermesRuntime) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: failed to create Hermes runtime for Fabric", EXPO_FABRIC_TAG);
        return false;
    }

    facebook::jsi::Runtime& runtime = *hermesRuntime;

    std::vector<std::string> viewManagers = {};
    RegisterFabricViewManagerImpl(runtime, "ExpoDeviceView");
    RegisterFabricViewManagerImpl(runtime, "ExpoFileSystemView");

    return true;
#else
    OH_LOG_INFO(LOG_APP, "%{public}s: Fabric view manager registration installed (N-API fallback)",
                EXPO_FABRIC_TAG);
    return true;
#endif
}

} // namespace expo
