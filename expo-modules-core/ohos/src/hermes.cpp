#include "hermes/hermes.h"

#include "jsi/jsi.h"

#include <cstring>
#include <stdexcept>

namespace facebook {
namespace hermes {

namespace {

class HermesPropNameID : public jsi::PropNameID {
public:
  explicit HermesPropNameID(std::string name) : name_(std::move(name)) {}

  bool operator==(const jsi::PropNameID& other) const override {
    auto otherUtf8 = other.utf8(jsi::Runtime::getDefaultJsAllocator());
    return otherUtf8 == name_;
  }

  bool operator!=(const jsi::PropNameID& other) const override {
    return !(*this == other);
  }

  std::string utf8(jsi::Runtime &) const override {
    return name_;
  }

private:
  std::string name_;
};

class HermesString : public jsi::String {
public:
  explicit HermesString(std::string str) : str_(std::move(str)) {}

  std::string utf8(jsi::Runtime &) const override {
    return str_;
  }

private:
  std::string str_;
};

class HermesSymbol : public jsi::Symbol {
public:
  explicit HermesSymbol(std::string str) : str_(std::move(str)) {}

  std::string toString(jsi::Runtime &) const override {
    return str_;
  }

private:
  std::string str_;
};

class HermesArray : public jsi::Array {
public:
  explicit HermesArray(size_t size) : size_(size), values_(size) {}

  size_t size(jsi::Runtime &) const override {
    return size_;
  }

  jsi::Value getValueAtIndex(jsi::Runtime &, size_t i) const override {
    if (i >= size_) {
      throw std::out_of_range("Array index out of range");
    }
    return values_[i];
  }

  void setValueAtIndex(jsi::Runtime &, size_t i, jsi::Value value) override {
    if (i >= size_) {
      throw std::out_of_range("Array index out of range");
    }
    values_[i] = value;
  }

private:
  size_t size_;
  std::vector<jsi::Value> values_;
};

class HermesObject : public jsi::Object {
public:
  jsi::Value getProperty(jsi::Runtime &, const char* name) const override {
    auto it = properties_.find(name);
    if (it == properties_.end()) {
      return jsi::Value();
    }
    return it->second;
  }

  jsi::Value getProperty(jsi::Runtime &, const std::shared_ptr<jsi::PropNameID>& name) const override {
      return getProperty(jsi::Runtime::getDefaultJsAllocator(), name->utf8(jsi::Runtime::getDefaultJsAllocator()).c_str());
  }

  void setProperty(jsi::Runtime &, const char* name, jsi::Value value) override {
    properties_[name] = value;
  }

  void setProperty(jsi::Runtime &, const std::shared_ptr<jsi::PropNameID>& name, jsi::Value value) override {
    setProperty(jsi::Runtime::getDefaultJsAllocator(), name->utf8(jsi::Runtime::getDefaultJsAllocator()).c_str(), value);
  }

  bool hasProperty(jsi::Runtime &, const char* name) const override {
    return properties_.find(name) != properties_.end();
  }

  bool hasProperty(jsi::Runtime &, const std::shared_ptr<jsi::PropNameID>& name) const override {
    return hasProperty(jsi::Runtime::getDefaultJsAllocator(), name->utf8(jsi::Runtime::getDefaultJsAllocator()).c_str());
  }

  std::vector<std::shared_ptr<jsi::PropNameID>> getPropertyNames(jsi::Runtime &) const override {
    std::vector<std::shared_ptr<jsi::PropNameID>> names;
    for (const auto& [key, _] : properties_) {
      names.push_back(std::make_shared<HermesPropNameID>(key));
    }
    return names;
  }

  bool isArray(jsi::Runtime &) const override {
    return false;
  }

  bool isFunction(jsi::Runtime &) const override {
    return false;
  }

  std::shared_ptr<jsi::Array> getArray(jsi::Runtime &) override {
    return nullptr;
  }

  std::shared_ptr<jsi::Function> getFunction(jsi::Runtime &) override {
    return nullptr;
  }

  std::shared_ptr<jsi::Object> getArrayBuffer(jsi::Runtime &) override {
    return nullptr;
  }

  bool hasNativeState(jsi::Runtime &) override {
    return false;
  }

private:
  std::unordered_map<std::string, jsi::Value> properties_;
};

class HermesFunction : public jsi::Function {
public:
  explicit HermesFunction(HermesRuntime::HostFunctionType func) : func_(std::move(func)) {}

  jsi::Value call(
      jsi::Runtime & runtime,
      const jsi::Value& thisValue,
      const jsi::Value* args,
      size_t count) override {
    return func_(static_cast<HermesRuntime&>(runtime), thisValue, args, count);
  }

  jsi::Value call(
      jsi::Runtime & runtime,
      const jsi::Value& thisValue,
      const std::vector<jsi::Value>& args) override {
    return call(runtime, thisValue, args.data(), args.size());
  }

  jsi::Value callAsConstructor(
      jsi::Runtime & runtime,
      const jsi::Value* args,
      size_t count) override {
    return func_(static_cast<HermesRuntime&>(runtime), jsi::Value(), args, count);
  }

private:
  HermesRuntime::HostFunctionType func_;
};

class HermesBuffer : public jsi::Buffer {
public:
  explicit HermesBuffer(std::string data) : data_(std::move(data)) {}

  size_t size() const override {
    return data_.size();
  }

  const uint8_t* data() const override {
    return reinterpret_cast<const uint8_t*>(data_.data());
  }

private:
  std::string data_;
};

} // namespace

std::unique_ptr<HermesRuntime> HermesRuntime::create(
    std::unique_ptr<jsi::RuntimeState> state) {
  return std::unique_ptr<HermesRuntime>(new HermesRuntime());
}

bool HermesRuntime::isHermesRuntime() const {
  return true;
}

std::shared_ptr<jsi::Object> HermesRuntime::global() {
  auto global = std::make_shared<HermesObject>();
  properties_["expo"] = jsi::Value(global);
  return global;
}

jsi::Value HermesRuntime::evaluate(const char* code, const char* sourceUrl) {
  (void)code;
  (void)sourceUrl;
  return jsi::Value();
}

jsi::Value HermesRuntime::evaluate(const std::shared_ptr<const jsi::String>& code, const char* sourceUrl) {
  (void)code;
  (void)sourceUrl;
  return jsi::Value();
}

jsi::Value HermesRuntime::evaluate(std::shared_ptr<jsi::Buffer>, const char* sourceUrl) {
  (void)sourceUrl;
  return jsi::Value();
}

void HermesRuntime::scheduleMicrotask() {}

jsi::Value HermesRuntime::getProperty(const std::shared_ptr<jsi::Object>& obj, const char* name) {
  return obj->getProperty(jsi::Runtime::getDefaultJsAllocator(), name);
}

void HermesRuntime::setProperty(const std::shared_ptr<jsi::Object>& obj, const char* name, jsi::Value value) {
  obj->setProperty(jsi::Runtime::getDefaultJsAllocator(), name, value);
}

bool HermesRuntime::hasProperty(const std::shared_ptr<jsi::Object>& obj, const char* name) const {
    return false;
}

std::shared_ptr<jsi::PropNameID> HermesRuntime::getSymbol(const char* utf8) {
  return std::make_shared<HermesPropNameID>(utf8);
}

std::shared_ptr<jsi::Object> HermesRuntime::createObject() {
  return std::make_shared<HermesObject>();
}

std::shared_ptr<jsi::Object> HermesRuntime::createObject(std::shared_ptr<jsi::RuntimeState>) {
  return createObject();
}

std::shared_ptr<jsi::Array> HermesRuntime::createArray(size_t length) {
  return std::make_shared<HermesArray>(length);
}

std::shared_ptr<jsi::String> HermesRuntime::createString(const char* str) {
  return std::make_shared<HermesString>(str);
}

std::shared_ptr<jsi::String> HermesRuntime::createString(const std::string& str) {
  return std::make_shared<HermesString>(str);
}

std::shared_ptr<jsi::Symbol> HermesRuntime::createSymbol(const char* str) {
  return std::make_shared<HermesSymbol>(str);
}

std::shared_ptr<jsi::Function> HermesRuntime::createFunctionFromHostFunction(
    const char* name,
    unsigned int paramCount,
    HostFunctionType func) {
  (void)paramCount;
  (void)name;
  return std::make_shared<HermesFunction>(func);
}

std::shared_ptr<jsi::Object> HermesRuntime::getNativeState(Runtime&, std::shared_ptr<jsi::RuntimeState>) {
  return nullptr;
}

bool HermesRuntime::isInspectable() {
  return false;
}

} // namespace hermes
} // namespace facebook
