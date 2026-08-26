#ifndef EXPOMODULESCORE_HERMES_ENGINE_H
#define EXPOMODULESCORE_HERMES_ENGINE_H

#include <jsi/jsi.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace facebook {
namespace hermes {

class HermesRuntime : public jsi::Runtime {
public:
  static std::unique_ptr<HermesRuntime> create(
      std::unique_ptr<jsi::RuntimeState> state = nullptr);

  ~HermesRuntime() override = default;

  bool isHermesRuntime() const;

  std::shared_ptr<jsi::Object> global() override;
  jsi::Value evaluate(const char* code, const char* sourceUrl = nullptr) override;
  jsi::Value evaluate(const std::shared_ptr<const jsi::String>& code, const char* sourceUrl = nullptr) override;
  jsi::Value evaluate(std::shared_ptr<jsi::Buffer>, const char* sourceUrl = nullptr) override;
  void scheduleMicrotask() override;
  jsi::Value getProperty(const std::shared_ptr<jsi::Object>&, const char* name) override;
  void setProperty(const std::shared_ptr<jsi::Object>&, const char* name, jsi::Value) override;
  bool hasProperty(const std::shared_ptr<jsi::Object>&, const char* name) const override;
  std::shared_ptr<jsi::PropNameID> getSymbol(const char* utf8) override;
  std::shared_ptr<jsi::Object> createObject() override;
  std::shared_ptr<jsi::Object> createObject(std::shared_ptr<jsi::RuntimeState>) override;
  std::shared_ptr<jsi::Array> createArray(size_t length) override;
  std::shared_ptr<jsi::String> createString(const char*) override;
  std::shared_ptr<jsi::String> createString(const std::string&) override;
  std::shared_ptr<jsi::Symbol> createSymbol(const char*) override;
  std::shared_ptr<jsi::Function> createFunctionFromHostFunction(
      const char* name,
      unsigned int paramCount,
      HostFunctionType func) override;
  std::shared_ptr<jsi::Object> getNativeState(Runtime&, std::shared_ptr<jsi::RuntimeState>) override;
  bool isInspectable() override;

private:
  std::unordered_map<std::string, std::shared_ptr<jsi::Object>> objects_;
  std::unordered_map<std::string, std::shared_ptr<jsi::Function>> functions_;
  std::unordered_map<std::string, jsi::Value> properties_;
};

} // namespace hermes
} // namespace facebook

#endif
