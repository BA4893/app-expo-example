export interface ExpoDeviceModule {
  getModelName: () => string;
  getBrand: () => string;
  getMarketName: () => string;
  getDeviceType: () => string;
  getOsName: () => string;
  getOsVersion: () => string;
  getSdkApiVersion: () => number;
}
export declare const ExpoDevice: ExpoDeviceModule;
