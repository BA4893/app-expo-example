---
name: conductor-dev
description: Professional project orchestration and workflow management. Initialize the Conductor directory to manage tracks, specifications, and implementation plans.
---

# Conductor Dev

## Overview

This skill provides the boilerplate files and structure for the Conductor project orchestration framework. It initializes a `conductor/` directory with essential documentation and workflow definitions according to the Universal File Resolution Protocol. It supports both standard linear workflows and advanced cycle-based development (V4 Supercharged).

Conductor owns the **orchestration layer**. For *how* to build, lint, deploy, and validate
OpenHarmony apps it defers to **`ohos-app-dev`** (the inner loop) and **`ohos-app-scaffold`**
(project creation) — it does not restate `oniro-app` toolchain commands.

## Workflow

### 1. Initialize Conductor
Scaffolds the `conductor/` directory in the current project.
- **Trigger:** "Initialize conductor", "Setup conductor template", "Add conductor to project"
- **Action:**
  1. Create a `conductor/` directory at the project root if it doesn't exist.
  2. Create an `artifacts/snapshots/` directory at the project root if it doesn't exist.
  3. Create an `artifacts/logs/` directory at the project root if it doesn't exist.
  4. Copy all template files from `assets/conductor-template/` to the `conductor/` directory using a cross-platform recursive copy (the agent's file tools, or `cp -r` on macOS/Linux / `Copy-Item -Recurse` on Windows).
  5. Inform the user that the Conductor directory and artifacts folder have been initialized.

### 2. Cycle Workflow (V4 Supercharged)
Executes a structured task cycle defined in `conductor/workflows/cycle.md`.
- **Key Files:** `TASKS.md`, `SESSION_STATE.json` (runtime), `artifacts/logs/`
- **Design Context:** Use the **Figma Extension** to inspect designs and extract CSS/ArkTS styling before implementation.
- **Steps:** Initialization -> Design Analysis (Figma) -> Strategy -> Execution -> Self-Review -> Synchronization -> Handover.

## Implementation Directives

When implementing tracks or tasks under Conductor:

1. **Design-First implementation:** For all UI tasks, you MUST use the `figma` extension to fetch and analyze the relevant design files. Extract precise values for dimensions, colors, and typography to ensure pixel-perfect implementation.
2. **Dynamic Path Resolution:** ALWAYS resolve template paths relative to the skill's root directory.
3. **Toolchain → `ohos-app-dev`.** Lint, build, deploy, logs, screenshots, and UI inspection all run through the cross-platform `oniro-app` CLI as documented in **`ohos-app-dev`** — do not hand-roll `codelinter` / `hvigorw` / `hdc`. In particular, lint changed files with `oniro-app lint` before committing.
4. **Task Verification:**
    - For logic changes, rely primarily on `oniro-app lint` to save tokens.
    - **UI Validation (MANDATORY):** for any UI modification you MUST capture the screen into the track artifact and compare it against the Figma design:
        1. Ensure a device/emulator is connected (`oniro-app devices`) and `artifacts/snapshots/` exists.
        2. `oniro-app screenshot -o "artifacts/snapshots/[task_id].jpeg"` — add `--grid` to read coordinates off a 10×10 (0.0–1.0) overlay, or `--contact-sheet` for a transient/animated state.
        3. Note any discrepancies vs the design in `conductor/learning.md` or the log.
      (See `ohos-app-dev` for the full screenshot / dump / input surface.)
5. **Visual Proof:** Save all UI validation snapshots to `artifacts/snapshots/[task_id].png` (or `[task_id]_test.png`).
6. **Idempotent Edits:** Before using the `replace` tool, you MUST use `read_file` to verify the current content. Ensure that `new_string` is DIFFERENT from the existing text to avoid "No changes to apply" errors.
7. **Automated Self-Learning Loop:**
    - At the start of every track implementation or task, you MUST read `conductor/learning.md`. If it does not exist, create it with a `# Learning Log` header.
    - If an error happens, analyze and document the fix in `conductor/learning.md`.
    - **Display Lessons:** Proactively notify the user and explicitly display the content of "Lessons Learned" in the chat whenever `conductor/learning.md` is updated or after resolving errors or complex tasks.
8. **Flexible Checkpointing:** Commit changes after every task by default, but allow "Phase-based" commits if requested in the track plan.
9. **Quality Standards:** Adhere strictly to the project's `product-guidelines.md` and prioritize UI performance ("Premium Design" and "Robustness"). ArkTS strict-mode rules and app architecture live in `ohos-app-dev/references/`.
10. **HarmonyOS Context:** Read `build-profile.json5` for SDK versions; run `oniro-app sdk list` for the version↔API-level mapping.
11. **Final Build Verification:** ONLY after all phases in a track's `plan.md` are completed, you MUST run `oniro-app build` as a final verification that the whole project is in a stable, buildable state before completing the track. Do not run it after individual tasks or phases.

## Resources

### assets/
Contains the template files for the Conductor directory structure:
- `index.md`: The entry point and file resolution index.
- `product.md`: Product definition and goals.
- `tech-stack.md`: Technology stack and architectural decisions.
- `workflow.md`: Development workflow and lifecycle.
- `tracks.md`: Registry for development tracks.
- `product-guidelines.md`: Design and implementation guidelines.
- `setup_state.json`: Initial state configuration for setup tracking.
- `code_styleguides/`: Directory for language-specific style guides (e.g., `arkts.md`).
- `workflows/`: Detailed workflow patterns (e.g., `cycle.md` for V4 Supercharged cycle).

### references/
- `agency-workflow.md`: The orchestration framework's lifecycle model — the "Golden Trio" (`TASKS.md`, `SESSION_STATE.json`, `cycle.md`) and the `/cycle` phases.

## SDK / API levels

Run `oniro-app sdk list` for the version↔API-level mapping. If `build-profile.json5` shows the API level in parentheses (e.g. `6.0.0(20)`), that number is the API level.
