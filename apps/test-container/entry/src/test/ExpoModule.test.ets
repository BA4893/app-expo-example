import { describe, it, expect } from '@ohos/hypium';
import expo from 'libentry.so';

export default function expoModuleTest() {
  describe('ExpoModulesNative', () => {
    it('expoDevice_returns_string_model', 0, () => {
      const model = expo.ExpoDevice.getModelName();
      expect(model).assertEqual(typeof model === 'string');
    });

    it('expoDevice_returns_string_brand', 0, () => {
      const brand = expo.ExpoDevice.getBrand();
      expect(typeof brand).assertEqual('string');
    });

    it('expoDevice_osName_is_openharmony', 0, () => {
      const osName = expo.ExpoDevice.getOsName();
      expect(osName).assertEqual('OpenHarmony');
    });

    it('expoDevice_sdkApiVersion_is_number', 0, () => {
      const api = expo.ExpoDevice.getSdkApiVersion();
      expect(typeof api).assertEqual('number');
    });

    it('expoFileSystem_write_read_roundtrip', 0, async () => {
      const path = '/data/storage/el2/base/haps/entry/files/unit-test.txt';
      await expo.ExpoFileSystem.writeFile(path, 'unit');
      const contents = await expo.ExpoFileSystem.readFile(path);
      expect(contents).assertEqual('unit');
    });
  });
}
