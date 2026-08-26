#ifndef EXPOMODULESCORE_REGISTRY_H
#define EXPOMODULESCORE_REGISTRY_H

#include "napi/native_api.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace expo {

using MethodCallback = std::function<napi_value(napi_env, napi_callback_info)>;

struct ModuleDefinition {
    const char *name;
    std::vector<std::pair<const char *, MethodCallback>> methods;
};

// Explicit, static registry. ArkTS strict mode forbids runtime shape mutation and
// reflection, so every module registers a plain name -> (method -> callback) table.
class ExpoModulesRegistry {
public:
    void Register(const ModuleDefinition &definition);

    // Exports a JS object for each registered module onto `exports`.
    napi_value ExportAll(napi_env env, napi_value exports);

    const ModuleDefinition *Find(const std::string &name) const;

private:
    std::vector<ModuleDefinition> modules_;
};

} // namespace expo

#endif
