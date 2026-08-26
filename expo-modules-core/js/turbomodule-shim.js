(function () {
  var g = (typeof globalThis !== 'undefined') ? globalThis : global;

  if (!g.expo) { g.expo = {}; }

  var TurboModuleRegistry = {};
  g.expo.TurboModuleRegistry = TurboModuleRegistry;

  function getter(moduleName) {
    return function () {
      var mod = g.requireOptionalNativeModule && g.requireOptionalNativeModule(moduleName);
      if (!mod) {
        throw new Error('TurboModule "' + moduleName + '" is not available');
      }
      return mod;
    };
  }

  g.createTurboModule = function (moduleName) {
    return getter(moduleName)();
  };

  g.requireNativeModule = g.requireNativeModule || function (moduleName) {
    var mod = g.requireOptionalNativeModule && g.requireOptionalNativeModule(moduleName);
    if (!mod) {
      throw new Error('Cannot find native module "' + moduleName + '"');
    }
    return mod;
  };
})();
