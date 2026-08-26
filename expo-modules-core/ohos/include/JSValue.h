#ifndef EXPOMODULESCORE_JSVALUE_H
#define EXPOMODULESCORE_JSVALUE_H

#include "napi/native_api.h"

#include <string>

namespace expo {

// Tagged value model over napi_valuetype. Kept small and explicit to stay
// compatible with ArkTS strict mode (no runtime shape changes).
enum class JSValueType {
  Undefined,
  Null,
  Boolean,
  Number,
  String,
  ArrayBuffer,
  Object,
  Function,
};

class JSValue {
public:
  JSValueType type = JSValueType::Undefined;

  bool boolean = false;
  double number = 0.0;
  std::string string;
  void *data = nullptr;
  size_t length = 0;
  napi_ref ref = nullptr; // retained reference for Object/Function
};

// Reads a napi_value into a JSValue, retaining object/function references.
bool ReadJSValue(napi_env env, napi_value value, JSValue &out);

// Releases any retained reference held by a JSValue.
void ReleaseJSValue(napi_env env, JSValue &value);

} // namespace expo

#endif
