// Minimal Expo JS runtime shim, matching the expo-modules-core contract.
// Installed on globalThis so a Metro bundle can call requireNativeModule(...).
(function () {
  var g = (typeof globalThis !== 'undefined') ? globalThis : global;

  if (!g.expo) { g.expo = {}; }
  if (!g.expo.modules) { g.expo.modules = {}; }

  // NativeModule base: a tagged host object. Real Expo implements this in C++/JSI;
  // here it's an identity marker so `instanceof NativeModule` behaves sanely.
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
})();
