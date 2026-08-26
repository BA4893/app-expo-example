# HarmonyOS Clean Architecture Guide

## Overview
This project follows a strict "Clean Architecture" pattern to ensure maintainability, testability, and scalability.

## Layers

### 1. Domain Layer (Core)
- **Role:** Contains the business logic and entities.
- **Dependencies:** NONE. Must be pure ArkTS/TypeScript.
- **Components:**
  - **Entities:** Data models representing business objects.
  - **Use Cases:** Specific business actions (e.g., `GetTaskStatsUseCase`).
  - **Repository Interfaces:** Contracts defining data access methods.

### 2. Data Layer (Infrastructure)
- **Role:** Handles data retrieval and storage.
- **Dependencies:** Domain Layer.
- **Components:**
  - **Repository Implementations:** Concretions of Domain interfaces.
  - **Data Sources:** API clients, local database (RelationalStore/Preferences).
  - **Mappers:** Convert "Data Models" (JSON/DB) to "Domain Entities".

### 3. Presentation Layer (UI)
- **Role:** Displays data and handles user interaction.
- **Dependencies:** Domain Layer.
- **Components:**
  - **ViewModels:** Manages UI state and executes Use Cases.
  - **ArkUI Components:** Declarative UI using `@Component`, `@State`, `@Link`.

## Rules
1. **Dependency Rule:** Source code dependencies can only point inwards. (UI -> Domain <- Data).
2. **Separation:** The UI should never call the Data layer directly. It must go through the Domain (Use Cases).
3. **State:** Use ArkTS decorators (`@State`, `@Prop`) strictly for UI state. Business logic state belongs in the ViewModel or Domain.
