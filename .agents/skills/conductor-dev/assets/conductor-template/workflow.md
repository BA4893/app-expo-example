# Project Workflow

## Development Process
1. **Track Definition:** Every major piece of work starts as a Track in `conductor/tracks/`.
2. **Specification & Planning:** Each track must have a `spec.md` and a `plan.md` before implementation begins.
3. **Implementation:**
    - Work is performed in phases as defined in the `plan.md`.
    - **Lint Before Commit:** Run `oniro-app lint` on all modified files immediately after editing and before running tests.
    - Each task must be verified with tests where applicable.
    - **Visual Snapshots:** For every UI-related task, capture a screenshot or visual proof. Save to `conductor/artifacts/snapshots/[task_id].png`.
4. **Verification:**
    - **Task-level Verification:** Ensure `oniro-app lint` passes and unit tests are successful for the specific changes in each task.
    - **Final Build Check:** After ALL phases in the track plan are finished, run `oniro-app build` to ensure the entire project is stable and buildable.
    - **Test Coverage:** Maintain at least **80%** test coverage.
    - **Visual Regression:** Compare new snapshots against baseline if applicable.
5. **Checkpointing:**
    - **Commit Strategy:** Default is committing after every **Task**. Optional: Commit after every **Phase**.
    - Use **Git Notes** to record task summaries for future reference.

## Quality & Learning
- **UI Optimization:** When implementing UI components, prioritize performance by minimizing nested containers and using efficient layout managers.
- **Self-Learning Loop:** 
    - When an error occurs during build, linting, or testing, document the root cause and the fix in `conductor/learning.md`.
    - At the end of each task, reflect on optimizations or reusable patterns.
- **Error Notification:** If a task fails or a build warning is detected, explicitly notify the user and suggest a "Self-Learning" update before proceeding.

## Environment & Tooling
- **OS Compatibility:** All build / lint / device actions go through the cross-platform **`oniro-app`** CLI (identical on Linux / macOS / Windows), so prefer it over raw shell. For raw shell commands, use syntax appropriate to the host OS.
- **PowerShell Best Practices (Windows):**
    - Use `;` instead of `&&` for command chaining.
    - Avoid CMD-specific flags like `/b`. Use `(Get-ChildItem -Name)` for bare listing.
    - Safely check for file existence using `Test-Path <file>` or `Get-ChildItem <file> -ErrorAction SilentlyContinue`.
    - Use `New-Item -ItemType Directory -Path <path> -Force` to create directories (equivalent to `mkdir -p`).
- **Git Context:** To avoid noise from parent repositories, always verify that Git operations are scoped to the current project directory. Use `.geminiignore` to strictly limit the analysis scope.

## Communication
- Use the Conductor files (`product.md`, `tracks.md`, etc.) as the source of truth for project status and requirements.
