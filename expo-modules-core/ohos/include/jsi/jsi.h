#ifndef EXPOMODULESCORE_JSI_H
#define EXPOMODULESCORE_JSI_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace facebook {
namespace jsi {

class Object;
class Array;
class String;
class Symbol;
class Function;
class Runtime;
class PropNameID;
class RuntimeState;
class Buffer;
class HostObject;

enum class JsiValueKind : uint32_t {
  Undefined = 0,
  Null,
  Boolean,
  Number,
  String,
  Symbol,
  Object,
  Array,
  Function,
};

class Value {
public:
  Value() : kind_(JsiValueKind::Undefined) {}
  Value(bool b) : kind_(JsiValueKind::Boolean), boolValue_(b) {}
  Value(int i) : kind_(JsiValueKind::Number), numberValue_(static_cast<double>(i)) {}
  Value(double d) : kind_(JsiValueKind::Number), numberValue_(d) {}
  Value(const String&) : kind_(JsiValueKind::String) {}
  Value(std::shared_ptr<Object> obj);
  Value(std::shared_ptr<String> obj);
  Value(std::shared_ptr<Function> obj);
  Value(std::shared_ptr<Array> obj);
  Value(std::shared_ptr<Symbol> obj);
  Value(std::nullptr_t) : kind_(JsiValueKind::Null) {}
  static Value undefined() { return Value(); }
  static Value null() { return Value(nullptr); }
  bool isBool() const { return kind_ == JsiValueKind::Boolean; }
  bool isNumber() const { return kind_ == JsiValueKind::Number; }
  bool isString() const { return kind_ == JsiValueKind::String; }
  bool isObject() const { return kind_ == JsiValueKind::Object; }
  bool isNull() const { return kind_ == JsiValueKind::Null; }
  bool isUndefined() const { return kind_ == JsiValueKind::Undefined; }
  bool isSymbol() const { return kind_ == JsiValueKind::Symbol; }
  bool isArray() const { return kind_ == JsiValueKind::Array; }
  bool isFunction() const { return kind_ == JsiValueKind::Function; }
  bool getBool() const { return boolValue_; }
  double getNumber() const { return numberValue_; }
  std::shared_ptr<Object> getObject(Runtime&) const { return objectValue_; }
  std::shared_ptr<String> getString(Runtime&) const { return nullptr; }
  std::shared_ptr<Function> getFunction(Runtime&) const { return nullptr; }
  std::shared_ptr<Array> getArray(Runtime&) const { return nullptr; }

private:
  JsiValueKind kind_;
  bool boolValue_ = false;
  double numberValue_ = 0.0;
  std::shared_ptr<Object> objectValue_;
};

class Runtime {
public:
  using HostFunctionType = std::function<Value(
      Runtime&,
      const Value& thisValue,
      const Value* args,
      size_t count)>;

  virtual ~Runtime() = default;
  virtual std::shared_ptr<Object> global() { return nullptr; }
  virtual Value evaluate(const char* code, const char* sourceUrl = nullptr) { (void)code; (void)sourceUrl; return Value(); }
  virtual Value evaluate(const std::shared_ptr<const String>& code, const char* sourceUrl = nullptr) { (void)code; (void)sourceUrl; return Value(); }
  virtual Value evaluate(std::shared_ptr<Buffer>, const char* sourceUrl = nullptr) { (void)sourceUrl; return Value(); }
  virtual void scheduleMicrotask() {}
  virtual Value getProperty(const std::shared_ptr<Object>&, const char* name) { (void)name; return Value(); }
  virtual void setProperty(const std::shared_ptr<Object>&, const char* name, Value val) { (void)name; (void)val; }
  virtual bool hasProperty(const std::shared_ptr<Object>&, const char* name) const { (void)name; return false; }
  virtual std::shared_ptr<PropNameID> getSymbol(const char* utf8) { return nullptr; }
  virtual std::shared_ptr<Object> createObject() { return nullptr; }
  virtual std::shared_ptr<Object> createObject(std::shared_ptr<RuntimeState>) { return nullptr; }
  virtual std::shared_ptr<Array> createArray(size_t) { return nullptr; }
  virtual std::shared_ptr<String> createString(const char*) { return nullptr; }
  virtual std::shared_ptr<String> createString(const std::string&) { return nullptr; }
  virtual std::shared_ptr<Symbol> createSymbol(const char*) { return nullptr; }
  virtual std::shared_ptr<Function> createFunctionFromHostFunction(
      const char* name,
      unsigned int paramCount,
      HostFunctionType func) { (void)name; (void)paramCount; (void)func; return nullptr; }
  virtual std::shared_ptr<Object> getNativeState(Runtime&, std::shared_ptr<RuntimeState>) { return nullptr; }
  virtual bool isInspectable() { return false; }
  static Runtime& getDefaultJsAllocator() {
    static Runtime instance;
    return instance;
  }
};

class RuntimeState {
public:
  virtual ~RuntimeState() = default;
};

class Buffer {
public:
  virtual ~Buffer() = default;
  virtual size_t size() const { return 0; }
  virtual const uint8_t* data() const { return nullptr; }
};

class PropNameID {
public:
  virtual ~PropNameID() = default;
  virtual bool operator==(const PropNameID&) const { return false; }
  virtual bool operator!=(const PropNameID& other) const { return !(*this == other); }
  virtual std::string utf8(Runtime &runtime) const { (void)runtime; return ""; }
  static std::shared_ptr<PropNameID> forUtf8(Runtime &runtime, const char* utf8);
};

class HostObject {
public:
  virtual ~HostObject() = default;
  virtual Value get(Runtime &rt, const PropNameID &name) = 0;
  virtual void set(Runtime &rt, const PropNameID &name, Value value) {}
  virtual std::vector<std::shared_ptr<PropNameID>> getPropertyNames(Runtime &rt) = 0;
};

class Object {
public:
  virtual ~Object() = default;
  virtual Value getProperty(Runtime &rt, const char* name) const { (void)rt; (void)name; return Value(); }
  virtual Value getProperty(Runtime &rt, const std::shared_ptr<PropNameID>& name) const { (void)rt; (void)name; return Value(); }
  virtual void setProperty(Runtime &rt, const char* name, Value val) { (void)rt; (void)name; (void)val; }
  virtual void setProperty(Runtime &rt, const std::shared_ptr<PropNameID>& name, Value val) { (void)rt; (void)name; (void)val; }
  virtual bool hasProperty(Runtime &rt, const char* name) const { (void)rt; (void)name; return false; }
  virtual bool hasProperty(Runtime &rt, const std::shared_ptr<PropNameID>& name) const { (void)rt; (void)name; return false; }
  virtual std::vector<std::shared_ptr<PropNameID>> getPropertyNames(Runtime &rt) const { (void)rt; return {}; }
  virtual bool isArray(Runtime &rt) const { (void)rt; return false; }
  virtual bool isFunction(Runtime &rt) const { (void)rt; return false; }
  virtual std::shared_ptr<Array> getArray(Runtime &rt) { (void)rt; return nullptr; }
  virtual std::shared_ptr<Function> getFunction(Runtime &rt) { (void)rt; return nullptr; }
  virtual std::shared_ptr<Object> getArrayBuffer(Runtime &rt) { (void)rt; return nullptr; }
  virtual bool hasNativeState(Runtime &rt) { (void)rt; return false; }
  static std::shared_ptr<Object> createFromHostObject(Runtime &rt, std::shared_ptr<HostObject> obj);
};

class String : public Object {
public:
  virtual ~String() = default;
  virtual std::string utf8(Runtime &rt) const { (void)rt; return ""; }
  static std::shared_ptr<String> createFromUtf8(Runtime &rt, const char *str) {
    (void)rt; (void)str; return nullptr;
  }
};

class Symbol : public Object {
public:
  virtual ~Symbol() = default;
  virtual std::string toString(Runtime &rt) const { (void)rt; return ""; }
  static std::shared_ptr<Symbol> forUtf8(Runtime &rt, const char *str);
};

class Array : public Object {
public:
  virtual ~Array() = default;
  virtual size_t size(Runtime &rt) const { (void)rt; return 0; }
  virtual Value getValueAtIndex(Runtime &rt, size_t i) const { (void)rt; (void)i; return Value(); }
  virtual void setValueAtIndex(Runtime &rt, size_t i, Value val) { (void)rt; (void)i; (void)val; }
};

class Function : public Object {
public:
  virtual ~Function() = default;
  virtual Value call(
      Runtime &rt,
      const Value& thisValue,
      const Value* args,
      size_t count) { (void)rt; (void)thisValue; (void)args; (void)count; return Value(); }
  virtual Value call(
      Runtime &rt,
      const Value& thisValue,
      const std::vector<Value>& args) { return call(rt, thisValue, args.data(), args.size()); }
  virtual Value callAsConstructor(
      Runtime &rt,
      const Value* args,
      size_t count) { (void)rt; (void)args; (void)count; return Value(); }
  static std::shared_ptr<Function> createFromHostFunction(
      Runtime &rt,
      const PropNameID &name,
      unsigned int paramCount,
      Runtime::HostFunctionType func);
};

inline std::shared_ptr<Object> Object::createFromHostObject(Runtime &, std::shared_ptr<HostObject>) {
    return nullptr;
}

inline std::shared_ptr<Function> Function::createFromHostFunction(
    Runtime &rt,
    const PropNameID &name,
    unsigned int paramCount,
    Runtime::HostFunctionType func) {
    return rt.createFunctionFromHostFunction(name.utf8(rt).c_str(), paramCount, func);
}

inline std::shared_ptr<PropNameID> PropNameID::forUtf8(Runtime &, const char*) {
    return nullptr;
}

inline std::shared_ptr<Symbol> Symbol::forUtf8(Runtime &, const char*) {
    return nullptr;
}

inline Value::Value(std::shared_ptr<Object> obj)
    : kind_(JsiValueKind::Object), objectValue_(std::move(obj)) {}

inline Value::Value(std::shared_ptr<String> obj)
    : Value(std::shared_ptr<Object>(obj)) {}

inline Value::Value(std::shared_ptr<Function> obj)
    : Value(std::shared_ptr<Object>(obj)) {}

inline Value::Value(std::shared_ptr<Array> obj)
    : Value(std::shared_ptr<Object>(obj)) {}

inline Value::Value(std::shared_ptr<Symbol> obj)
    : Value(std::shared_ptr<Object>(obj)) {}

} // namespace jsi
} // namespace facebook

namespace jsi = facebook::jsi;

#endif
