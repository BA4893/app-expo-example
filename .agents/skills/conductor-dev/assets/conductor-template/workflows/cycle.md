---
description: Automated Task Cycle from TASKS.md (V4 Supercharged)
---

// turbo-all

1. **Initialization**:
   - Read `SESSION_STATE.json` to understand environment context and previous lessons.
   - Read `TASKS.md` and identify the first incomplete task.
   - announce the selected task to the user.

2. **Analysis & Strategy**:
   - Analyze the existing codebase related to the task.
   - Create a technical execution plan and present it to the user.

3. **Execution**:
   - Implement the required changes in the code.
   - Ensure all new code adheres to the "Premium Design" guidelines (if UI) or "Robustness" (if logic).

4. **Self-Review & Verification**:
   - **Lint/Check**: Run `oniro-app lint` on the changed files.
   - **Test**: Run logic tests or visual checks.
   - **Snapshot**: For UI tasks, capture with `oniro-app screenshot -o artifacts/snapshots/[task_id].jpeg`.
   - **Critique**: Self-evaluate the work: "Did I break anything? Is there a more efficient way?"

5. **Synchronization**:
   - Mark the task as done in `TASKS.md` with [x].
   - Update `SESSION_STATE.json` with new dependencies, technical debt, and instructions for the next agent.
   - Log the change summary to `artifacts/logs/[task_id]_summary.txt`.

6. **Handover Protocol**:
   - Provide the user with a direct summary of changes.
   - Provide links to artifacts (logs/snapshots).
   - **Mandatory Final Action**: Ask the user to close this session and open a fresh agent session to maintain 100% context purity for the next task.
