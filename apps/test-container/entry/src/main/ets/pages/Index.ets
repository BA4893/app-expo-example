import { hilog } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';
import expo from 'libentry.so';

const DOMAIN = 0x0000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  aboutToAppear(): void {
    this.probeDevice();
    this.probeFileSystem();
  }

  probeDevice(): void {
    try {
      const model = expo.ExpoDevice.getModelName();
      const brand = expo.ExpoDevice.getBrand();
      const osName = expo.ExpoDevice.getOsName();
      const osVersion = expo.ExpoDevice.getOsVersion();
      hilog.info(DOMAIN, 'testTag', 'ExpoDevice model=%{public}s brand=%{public}s os=%{public}s %{public}s', model, brand, osName, osVersion);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'ExpoDevice probe failed: %{public}s', JSON.stringify(err));
    }
  }

  getFilesDir(): string {
    const ctx = this.getUIContext().getHostContext() as common.UIAbilityContext;
    return ctx.filesDir;
  }

  async probeFileSystem(): Promise<void> {
    try {
      const filesDir = this.getFilesDir();
      const testPath = filesDir + '/expo-test.txt';
      const testDir = filesDir + '/expo-test-dir';

      await expo.ExpoFileSystem.writeFile(testPath, 'hello from expo-file-system');
      const contents = await expo.ExpoFileSystem.readFile(testPath);
      const stat = await expo.ExpoFileSystem.statFile(testPath);
      await expo.ExpoFileSystem.makeDirectory(testDir);
      const listing = await expo.ExpoFileSystem.listDirectory(filesDir);
      await expo.ExpoFileSystem.deleteFile(testPath);

      hilog.info(DOMAIN, 'testTag', 'ExpoFileSystem contents=%{public}s size=%{public}d dirCount=%{public}d', contents, stat.size, listing.length);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'ExpoFileSystem probe failed: %{public}s', JSON.stringify(err));
    }
  }

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize($r('app.float.page_text_font_size'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = 'Welcome';
            this.probeDevice();
            this.probeFileSystem();
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}