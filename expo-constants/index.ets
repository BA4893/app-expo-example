export interface ExpoConstantsModule {
  getPlatformName: () => string;
  getSdkVersion: () => string;
  getStatusBarHeight: () => number;
  getDeviceYearClass: () => number;
}

export interface Constants {
  platformName: string;
  sdkVersion: string;
  statusBarHeight: number;
  deviceYearClass: number;
}

export const ExpoConstants = {
  platformName: 'OpenHarmony',
  sdkVersion: '1.0.0',
  statusBarHeight: 24,
  deviceYearClass: 2023,
} as Constants;

export default ExpoConstants;
