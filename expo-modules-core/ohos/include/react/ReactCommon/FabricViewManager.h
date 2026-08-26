#ifndef EXPOMODULESCORE_FABRIC_VIEW_MANAGER_H
#define EXPOMODULESCORE_FABRIC_VIEW_MANAGER_H

#include <jsi/jsi.h>
#include <string>
#include <unordered_map>

namespace facebook {
namespace react {

class ViewManager {
public:
  virtual ~ViewManager() = default;
  virtual std::string getName() const = 0;
  virtual std::shared_ptr<jsi::Object> createView(
      jsi::Runtime& runtime,
      const std::shared_ptr<jsi::Object>& props) = 0;
};

class ViewManagerRegistry {
public:
  virtual ~ViewManagerRegistry() = default;
  virtual void registerViewManager(std::shared_ptr<ViewManager> viewManager) = 0;
  virtual std::shared_ptr<ViewManager> getViewManager(const std::string& name) const = 0;
};

class FabricViewManagerFacade {
public:
  static FabricViewManagerFacade& getInstance() {
    static FabricViewManagerFacade instance;
    return instance;
  }

  bool registerViewManager(jsi::Runtime& runtime, std::shared_ptr<ViewManager> viewManager) {
    (void)runtime;
    if (!viewManager) {
      return false;
    }
    viewManagers_[viewManager->getName()] = std::move(viewManager);
    return true;
  }

  std::shared_ptr<ViewManager> getViewManager(jsi::Runtime& runtime, const std::string& name) const {
    (void)runtime;
    auto it = viewManagers_.find(name);
    if (it != viewManagers_.end()) {
      return it->second;
    }
    return nullptr;
  }

private:
  FabricViewManagerFacade() = default;
  std::unordered_map<std::string, std::shared_ptr<ViewManager>> viewManagers_;
};

} // namespace react
} // namespace facebook

#endif
