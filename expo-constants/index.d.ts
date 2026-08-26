export interface Constants {
  platformName: string;
  sdkVersion: string;
  statusBarHeight: number;
  deviceYearClass: number;
}

export interface ExpoConstantsModule {
  getPlatformName: () => string;
  getSdkVersion: () => string;
  getStatusBarHeight: () => number;
  getDeviceYearClass: () => number;
}

export declare const ExpoConstants: Constants;
export declare const ExpoConstantsNative: ExpoConstantsModule;
