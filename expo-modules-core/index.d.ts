export interface ExpoModulesRegistryExports {}

export interface JsiModuleDefinition {
  name: string;
  methods: JsiMethodDefinition[];
}

export interface JsiMethodDefinition {
  name: string;
  callback: (...args: any[]) => any;
}

export declare const ExpoModulesCore: ExpoModulesRegistryExports;

export declare const ExpoJsiBridge: {
  Instance(): {
    RegisterModule(definition: JsiModuleDefinition): void;
    InstallIntoRuntime(runtime: any): boolean;
    Find(name: string): JsiModuleDefinition | undefined;
  };
};
