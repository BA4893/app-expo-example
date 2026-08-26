# Product Guidelines

> **Note:** These are the baseline guidelines for HarmonyOS development. They will be refined and expanded during the interactive project setup.

## Architectural Principles
- **Clean Architecture:** Strict separation of concerns between Domain, Data, and Presentation layers. The Domain layer must remain independent of external frameworks.
- **Dependency Rule:** Dependencies must only point inwards. The Domain layer should not know about the Data or Presentation layers.
- **Single Responsibility:** Each module, class, and function should have one clear responsibility to ensure maintainability.

## Development Workflow
- **Build Verification:** Run `oniro-app build` after any structural change or new feature implementation to ensure build integrity.
- **Test-Driven Development (TDD):** Encourage writing unit tests for business logic in the Domain layer and UI tests for critical user flows.
- **Continuous Integration:** Ensure the project remains in a "buildable" state at all times.

## ArkTS Development Standards
*Based on OpenHarmony/Oniro application development concepts.*
- **Declarative UI:** Build UIs using the declarative syntax. Decompose complex views into smaller, reusable custom components (`@Component`) to maintain a clean UI tree.
- **State Management:** Use the correct decorators for data flow:
    - `@State` for internal component state.
    - `@Prop` for one-way synchronization from parent.
    - `@Link` for two-way synchronization.
    - `@Provide` / `@Consume` for cross-component communication.
- **UIAbility Lifecycle:** Manage application entry points and lifecycle events (Create, Foreground, Background, Destroy) within the `UIAbility` context properly.
- **Strict Typing:** Leverage TypeScript's static typing to prevent runtime errors. Avoid using `any` unless absolutely necessary.

## Code Standards
- **Naming Conventions:** Follow HarmonyOS/ArkTS official naming conventions (CamelCase for classes/structs, lowerCamelCase for variables/functions).
- **Documentation:** Every public API and complex logic block must be documented to assist developers using this template.

## Visual & UX Standards
- **Consistency:** UI components should follow the project's established design patterns.
- **Responsiveness:** Components should be designed to handle multiple device types (phone, wearable) as specified in the project configuration.
