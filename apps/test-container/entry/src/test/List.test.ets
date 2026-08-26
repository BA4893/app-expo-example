import localUnitTest from './LocalUnit.test';
import expoModuleTest from './ExpoModule.test';

export default function testsuite() {
  localUnitTest();
  expoModuleTest();
}