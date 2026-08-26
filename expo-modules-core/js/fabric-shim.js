(function () {
  var g = (typeof globalThis !== 'undefined') ? globalThis : global;

  if (!g.expo) { g.expo = {}; }

  var fabricComponentRegistry = {};
  g.expo.Fabric = fabricComponentRegistry;

  g.requireNativeComponent = function (componentName) {
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
  };

  g.expo.registerFabricComponent = function (componentName, componentClass) {
    fabricComponentRegistry[componentName] = componentClass;
  };
})();
