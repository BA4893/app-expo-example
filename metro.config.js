const { getDefaultConfig, mergeConfig } = require('@react-native/metro-config');
const { resolver: expoResolver } = require('expo-modules-core/metro-config');

const config = getDefaultConfig(__dirname);

config.resolver.sourceExts = [
  ...config.resolver.sourceExts,
  'mjs',
];

config.resolver.assetExts = [
  ...config.resolver.assetExts,
  'ets',
  'hml',
];

config.transformer.getTransformOptions = async () => ({
  transform: {
    experimentalImportSupport: false,
    inlineRequires: true,
  },
});

config.watchFolders = [
  __dirname,
  __dirname + '/expo-modules-core',
  __dirname + '/expo-device',
  __dirname + '/expo-file-system',
];

module.exports = mergeConfig(config, {
  projectRoot: __dirname,
});
