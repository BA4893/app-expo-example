(function () {
  var g = (typeof globalThis !== 'undefined') ? globalThis : global;

  if (!g.expo) { g.expo = {}; }

  var viewManagerRegistry = {};
  g.expo.ViewManagers = viewManagerRegistry;

  g.requireNativeView = function (componentName) {
    var cached = viewManagerRegistry[componentName];
    if (cached) {
      return cached;
    }

    function NativeViewManager() {
      throw new Error(
        'View manager "' + componentName + '" is not registered. ' +
        'Ensure FabricViewManagerFacade is populated on the native side.'
      );
    }

    viewManagerRegistry[componentName] = NativeViewManager;
    return NativeViewManager;
  };

  g.expo.registerViewManager = function (componentName, manager) {
    viewManagerRegistry[componentName] = manager;
  };
})();
