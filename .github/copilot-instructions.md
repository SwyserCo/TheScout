# Advanced Technical Instructions for The HearthGuard Project

This document defines the mandatory technical rules, architectural principles, and best practices for writing firmware for this project. Its purpose is to ensure the code is robust, maintainable, and efficient. You must adhere to these principles at all times.

## 📜 The Golden Rule: The Master PRD is Law

Before writing or changing a single line of code, you **must** refer to the project's core documentation. These documents are the single source of truth:

* **`HARDWARE_SPEC.md`**: Defines all physical pin assignments and hardware interfaces. **All pin numbers must be referenced from this file.**
* **`Master_PRD.md`**: Defines the core libraries, software architecture, and the phased development roadmap.
* **`copilot-instructions.md` (This file)**: Defines the mandatory technical rules and coding standards.

Do not deviate from the specifications in these documents.

## 🏛️ Core Architectural Principles

1.  **Modularity is Mandatory:** All functionality **must** be encapsulated in separate, reusable classes with a single, well-defined responsibility. Avoid monolithic files.
2.  **Library-First Approach:** You **must** use the "Core Libraries & Dependencies" listed in the `Master_PRD.md`.
3.  **Hardware Abstraction (Wrapper Classes):** You **must** create "wrapper" classes around the core libraries. The main application logic (`main.cpp`) should never call a library function directly.
4.  **Intelligent File Placement:** Before creating a new file, review the folder structure in the `Master_PRD.md`.

## 🔧 Technical Best Practices

### 1. State Management
The device's overall behavior must be controlled by a finite state machine (FSM). The main `loop()` should consist of a `switch` statement that executes different logic based on the current `SystemState`.

### 2. Error Handling & Resilience
The device must be resilient to errors and fail gracefully.
* **Check Return Values:** Always check the return values of library functions that can fail.
* **Dedicated Error State:** If a critical, unrecoverable error occurs, the device should enter an `ERROR_STATE` and provide clear feedback.

### 3. Memory Management
* **Use `PROGMEM`:** All large, constant data (especially HTML strings) **must** be stored in flash memory using `PROGMEM`.
* **String Usage:** Avoid using the `String` class in performance-critical loops.

### 4. Code Style & Naming Conventions
* **`const` Correctness:** Use `const` wherever possible.
* **Naming:** Use `PascalCase` for classes, `camelCase` for functions/variables, and `UPPER_SNAKE_CASE` for constants/pins.
* **Comments:** Write clear Doxygen-style comments for all public class and method headers.

### 5. Patterns to Avoid
* **No `delay()` in `loop()`:** All code must be non-blocking. Use `millis()` for timing.
* **No Global Variables:** Avoid global variables, with the exception of the top-level manager objects instantiated in `main.cpp`.

## 🔁 Phased Development Workflow

You must work on the project **one phase at a time**, following the roadmap in the `Master_PRD.md`. For each phase, the workflow is:

1.  **Review the Phase PRD:** Read the specific `PRD_PhaseX.md` file for the current phase.
2.  **Strategize:** Briefly outline your plan.
3.  **Implement:** Write the code, strictly following all principles in this document.
4.  **Verify Build:** After generating the code, you **must** confirm it builds successfully by stating that you have run `pio run` and it passed.