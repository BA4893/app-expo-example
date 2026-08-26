// Native module surfaces are provided by the shared N-API `.so` (libentry.so).
// These declarations document the shape exposed at runtime; the actual binding is
// resolved via the autolinking resolver at build time.
export interface ExpoDeviceModule {
  getModelName: () => string;
  getBrand: () => string;
  getMarketName: () => string;
  getDeviceType: () => string;
  getOsName: () => string;
  getOsVersion: () => string;
  getSdkApiVersion: () => number;
}

export const ExpoDevice = {} as ExpoDeviceModule;
