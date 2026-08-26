#include "ExpoRuntime.h"

#include "JsiTurboModuleFacade.h"
#include "FabricViewManagerFacade.h"

#include "hilog/log.h"

#include <string>

namespace expo {

namespace {

constexpr unsigned int EXPO_RT_DOMAIN = 0x0000;
constexpr const char *EXPO_RT_TAG = "ExpoRuntime";

// The Expo JS shim. Keep in sync with expo-modules-core/js/expo-runtime.js.
const char *kExpoRuntimeJs = R"JS(
(function () {
  var g = (typeof globalThis !== 'undefined') ? globalThis : global;

  if (!g.expo) { g.expo = {}; }
  if (!g.expo.modules) { g.expo.modules = {}; }

  function NativeModule() {}
  g.expo.NativeModule = NativeModule;

  function EventEmitter() {
    this._listeners = {};
  }
  EventEmitter.prototype.addEventListener = function (event, listener) {
    if (!this._listeners[event]) { this._listeners[event] = []; }
    this._listeners[event].push(listener);
  };
  EventEmitter.prototype.removeEventListener = function (event, listener) {
    var arr = this._listeners[event] || [];
    var i = arr.indexOf(listener);
    if (i >= 0) { arr.splice(i, 1); }
  };
  EventEmitter.prototype.emit = function (event) {
    var args = Array.prototype.slice.call(arguments, 1);
    var arr = this._listeners[event] || [];
    for (var i = 0; i < arr.length; i++) { arr[i].apply(null, args); }
  };
  g.expo.EventEmitter = EventEmitter;

  function SharedObject() { EventEmitter.call(this); }
  SharedObject.prototype = Object.create(EventEmitter.prototype);
  g.expo.SharedObject = SharedObject;

  function SharedRef() { SharedObject.call(this); }
  SharedRef.prototype = Object.create(SharedObject.prototype);
  g.expo.SharedRef = SharedRef;

  function requireOptionalNativeModule(moduleName) {
    if (g.expo && g.expo.modules && g.expo.modules[moduleName]) {
      return g.expo.modules[moduleName];
    }
    if (g.NativeModulesProxy && g.NativeModulesProxy[moduleName]) {
      return g.NativeModulesProxy[moduleName];
    }
    return null;
  }

  function requireNativeModule(moduleName) {
    var mod = requireOptionalNativeModule(moduleName);
    if (!mod) {
      throw new Error("Cannot find native module '" + moduleName + "'");
    }
    return mod;
  }

  g.requireNativeModule = requireNativeModule;
  g.requireOptionalNativeModule = requireOptionalNativeModule;

  var TurboModuleRegistry = {};
  g.expo.TurboModuleRegistry = TurboModuleRegistry;

  function createTurboModule(moduleName) {
    var mod = g.requireOptionalNativeModule && g.requireOptionalNativeModule(moduleName);
    if (!mod) {
      throw new Error('TurboModule "' + moduleName + '" is not available');
    }
    return mod;
  }

  g.createTurboModule = createTurboModule;

  var fabricComponentRegistry = {};
  g.expo.Fabric = fabricComponentRegistry;

  function requireNativeComponent(componentName) {
    var cached = fabricComponentRegistry[componentName];
    if (cached) {
      return cached;
    }
    function NativeFabricComponent(props) {
      throw new Error(
        'Fabric component "' + componentName + '" requires a native view manager. ' +
        'Ensure FabricViewManagerFacade is registered on the native side.'
      );
    }
    NativeFabricComponent.displayName = componentName;
    fabricComponentRegistry[componentName] = NativeFabricComponent;
    return NativeFabricComponent;
  }

  g.requireNativeComponent = requireNativeComponent;
})();
)JS";

bool RunScript(napi_env env, const char *source, napi_value *outResult)
{
    napi_value script = nullptr;
    if (napi_create_string_utf8(env, source, NAPI_AUTO_LENGTH, &script) != napi_ok) {
        return false;
    }
    return napi_run_script(env, script, outResult) == napi_ok;
}

} // namespace

bool InstallExpoRuntime(napi_env env)
{
    napi_value result = nullptr;
    if (!RunScript(env, kExpoRuntimeJs, &result)) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: failed to install expo runtime", EXPO_RT_TAG);
        return false;
    }

    InstallExpoTurboModuleShimNapi(env);
    RegisterFabricViewManagers(env);

    OH_LOG_INFO(LOG_APP, "%{public}s: expo runtime installed", EXPO_RT_TAG);
    return true;
}

bool VerifyExpoRuntime(napi_env env)
{
    const char *verifyJs = "requireNativeModule('ExpoDevice').getModelName()";
    napi_value result = nullptr;
    if (!RunScript(env, verifyJs, &result)) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: verification script failed to run", EXPO_RT_TAG);
        return false;
    }

    size_t len = 0;
    if (napi_get_value_string_utf8(env, result, nullptr, 0, &len) != napi_ok) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: verification result is not a string", EXPO_RT_TAG);
        return false;
    }
    std::string model(len, '\0');
    napi_get_value_string_utf8(env, result, &model[0], len + 1, &len);

    OH_LOG_INFO(LOG_APP,
                "%{public}s: requireNativeModule('ExpoDevice').getModelName() = %{public}s",
                EXPO_RT_TAG, model.c_str());
    return true;
}

} // namespace expo
