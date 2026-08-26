import { spawn } from 'node:child_process';
import path from 'node:path';

const PROJECT_ROOT = process.cwd();

async function runBuild() {
  console.log('[expo] Building HarmonyOS app...');
  const child = spawn('oniro-app', ['build', 'apps/test-container'], {
    cwd: PROJECT_ROOT,
    stdio: 'inherit',
    shell: process.platform === 'win32',
  });

  await new Promise((resolve, reject) => {
    child.on('exit', (code) => {
      if (code === 0) resolve();
      else reject(new Error(`Build failed with exit code ${code}`));
    });
  });

  console.log('[expo] Build complete.');
}

async function runMetro() {
  console.log('[expo] Starting Metro bundler...');
  const metroPath = path.join(PROJECT_ROOT, 'node_modules', '.bin', 'metro');
  const child = spawn(
    process.platform === 'win32' ? `${metroPath}.cmd` : metroPath,
    ['--config', path.join(PROJECT_ROOT, 'metro.config.js')],
    {
      cwd: PROJECT_ROOT,
      stdio: 'inherit',
      shell: process.platform === 'win32',
    }
  );

  child.on('error', (err) => {
    console.error('[expo] Metro failed to start:', err);
  });

  return child;
}

const command = process.argv[2] || 'build';

if (command === 'metro') {
  runMetro().catch((err) => {
    console.error(err);
    process.exit(1);
  });
} else {
  runBuild()
    .then(() => {
      console.log('[expo] You can now run Metro with: npm run metro');
    })
    .catch((err) => {
      console.error(err);
      process.exit(1);
    });
}
