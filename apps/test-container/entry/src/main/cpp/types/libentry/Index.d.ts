export interface ExpoDeviceModule {
  getModelName: () => string;
  getBrand: () => string;
  getMarketName: () => string;
  getDeviceType: () => string;
  getOsName: () => string;
  getOsVersion: () => string;
  getSdkApiVersion: () => number;
}

export interface FileStat {
  size: number;
  isDirectory: boolean;
  isFile: boolean;
}

export interface ExpoFileSystemModule {
  writeFile: (path: string, contents: string) => Promise<void>;
  readFile: (path: string) => Promise<string>;
  statFile: (path: string) => Promise<FileStat>;
  makeDirectory: (path: string) => Promise<void>;
  listDirectory: (path: string) => Promise<string[]>;
  deleteFile: (path: string) => Promise<void>;
}

export interface ExpoModules {
  ExpoDevice: ExpoDeviceModule;
  ExpoFileSystem: ExpoFileSystemModule;
}

declare const expo: ExpoModules;
export default expo;
