# Agentic Lifecycle Management: The Orchestration Framework

## 1. Overview
This framework ensures project continuity and context purity by shifting state management to a structured, file-based system. It is designed to handle both development and infrastructure tasks autonomously.

---

## 2. Core Components (The "Golden Trio")

### A. `TASKS.md` (The Strategic Backlog)
This is not just a checkbox list; it is a **Structured Specification**. Every task must follow this "Deep Context" format to ensure the agent has all necessary information:
- **Title**: Clear ID and summary (e.g., `TASK 01: ...`).
- **Context**: The "Why" – background information and business logic.
- **Technical Requirements**: Specific steps and low-level details.
- **Constraints**: Critical "guardrails" (e.g., "Vanilla CSS only", "Max 2Hz pulse", "No external APIs").
- **Acceptance Criteria**: Verifiable checkboxes that define the "Definition of Done".

### B. `SESSION_STATE.json` (The Technical Bridge)
Acts as the project's "Long-Term Memory" between sessions.
- **Environment**: OS details, installed tools, and environmental blockers (e.g., WSL vs Windows).
- **Technical Handover**: Lessons learned, injected dependencies, and critical hints for the next agent.

### C. `conductor/workflows/cycle.md` (The Engine)
Automates the lifecycle using the `/cycle` command. It uses `// turbo-all` for autonomous execution of terminal commands.

---

## 3. The Lifecycle Phases (The /cycle command)
1. **Initialization**: Read `SESSION_STATE.json` and `TASKS.md`. Identify the active task.
2. **Analysis**: Create an execution strategy based on the **Constraints** and **Requirements**.
3. **Execution**: Apply changes to the code or infrastructure.
4. **Validation**: Run automated tests and capture screenshots in `conductor/artifacts/snapshots/`.
5. **Synchronization**: Update `TASKS.md` (check off items) and `SESSION_STATE.json` (update state).
6. **Handover**: Provide a summary and ask the user to **start a fresh session** for the next task.

---

## 4. Best Practices for Infrastructure tasks
- **Diagnostics First**: Always start by checking the current state of the system and logging it to `artifacts/logs/`.
- **Idempotency**: Ensure that running the same task twice won't break the system.
- **Documentation**: If a new tool is installed in WSL, it **must** be added to the `environment` section of `SESSION_STATE.json`.

---

## 5. Security & Safety
- **Constraints are Mandatory**: Agents must respect all constraints defined in `TASKS.md` to avoid security or performance regressions.
- **Manual Peer Review**: The human user remains the final approver before a task is considered finalized in the larger project scope.