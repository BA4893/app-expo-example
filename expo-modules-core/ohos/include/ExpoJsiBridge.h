#ifndef EXPOMODULESCORE_EXPOJSIBRIDGE_H
#define EXPOMODULESCORE_EXPOJSIBRIDGE_H

#include "napi/native_api.h"

#include "jsi/jsi.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace expo {

using JsiHostFunction = std::function<jsi::Value(
    jsi::Runtime &runtime,
    const jsi::Value &thisValue,
    const jsi::Value *args,
    size_t count)>;

struct JsiMethodDefinition {
    const char *name;
    JsiHostFunction callback;
};

struct JsiModuleDefinition {
    const char *name;
    std::vector<JsiMethodDefinition> methods;
};

class ExpoJsiHostObject final : public facebook::jsi::HostObject {
public:
    explicit ExpoJsiHostObject(std::vector<JsiMethodDefinition> methods)
        : methods_(std::move(methods)) {}

    jsi::Value get(
        jsi::Runtime &runtime,
        const jsi::PropNameID &name) override {
        std::string propName = name.utf8(runtime);

        for (const auto &entry : methods_) {
            if (propName == entry.name) {
                return jsi::Function::createFromHostFunction(
                    runtime,
                    name,
                    0,
                    entry.callback);
            }
        }

        return jsi::Value::undefined();
    }

    void set(
        jsi::Runtime &runtime,
        const jsi::PropNameID &name,
        jsi::Value value) override {
        (void)runtime;
        (void)name;
        (void)value;
    }

    std::vector<std::shared_ptr<jsi::PropNameID>> getPropertyNames(
        jsi::Runtime &runtime) override {
        std::vector<std::shared_ptr<jsi::PropNameID>> names;
        for (const auto &entry : methods_) {
            names.push_back(jsi::PropNameID::forUtf8(runtime, entry.name));
        }
        return names;
    }

private:
    std::vector<JsiMethodDefinition> methods_;
};

class ExpoJsiBridge {
public:
    static ExpoJsiBridge &Instance();

    void RegisterModule(const JsiModuleDefinition &definition);

    bool InstallIntoRuntime(jsi::Runtime &runtime);

    const JsiModuleDefinition *Find(const std::string &name) const;

private:
    std::vector<JsiModuleDefinition> modules_;
};

} // namespace expo

#endif
