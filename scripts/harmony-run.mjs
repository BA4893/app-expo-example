import { spawn } from 'node:child_process';
import path from 'node:path';

const PROJECT_ROOT = process.cwd();

async function runMetro() {
  console.log('[expo] Starting Metro bundler for HarmonyOS...');
  const metroPath = path.join(PROJECT_ROOT, 'node_modules', '.bin', 'metro');
  const args = ['--config', path.join(PROJECT_ROOT, 'metro.config.js')];

  if (process.argv.includes('--dev')) {
    args.push('--dev', 'true');
  }

  const child = spawn(
    process.platform === 'win32' ? `${metroPath}.cmd` : metroPath,
    args,
    {
      cwd: PROJECT_ROOT,
      stdio: 'inherit',
      shell: process.platform === 'win32',
    }
  );

  child.on('error', (err) => {
    console.error('[expo] Metro failed to start:', err);
    console.error('[expo] Ensure dependencies are installed: npm install');
    process.exit(1);
  });

  return child;
}

runMetro().catch((err) => {
  console.error(err);
  process.exit(1);
});
