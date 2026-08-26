import { AppRegistry } from 'react-native';
import { name as appName } from './app.json';

// Register the main React Native component/ability
// This mirrors the standard RN entry but stays JS-only so Metro can bundle it.
function MainComponent() {
  return null;
}

AppRegistry.registerComponent(appName, () => MainComponent);

console.log('[expo] Metro entry registered for', appName);
