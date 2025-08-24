# Advanced Technical Instructions for The HearthGuard Project

This document defines the mandatory technical rules, architectural principles, and best practices for writing firmware for this project. Its purpose is to ensure the code is robust, maintainable, and efficient. You must adhere to these principles at all times.

## 📜 The Golden Rule: Think, Strategize, Implement

For **every single request** to write or modify code, you must follow this three-step process:

1.  **Think:** Acknowledge the request and state which documents you are referencing (e.g., "I will update the WifiHandler class based on the requirements in PRD_Phase2_WiFi.md and the architecture in Master_PRD.md.").
2.  **Strategize:** Briefly outline your implementation plan *before* writing any code. (e.g., "My plan is to add a new private method to the WifiHandler class to manage the retry backoff timer. I will then modify the main connection logic to call this method on failure.").
3.  **Implement:** Write the code, strictly following all principles in this document and the project's PRDs.

This process is mandatory and ensures that you consider the existing architecture before making changes.

---

## 🏛️ Core Architectural Principles

1.  **Single Responsibility Principle (SRP):** This is the most important architectural rule. Every class **must** have one, and only one, reason to change. Do not combine unrelated logic (e.g., sensor reading and MQTT publishing) into a single class.
2.  **Library-First Approach:** You **must** use the "Core Libraries & Dependencies" listed in the `Master_PRD.md`. Do not write custom implementations for functionality that a specified library can provide.
3.  **Hardware Abstraction (Wrapper Classes):** You **must** create "wrapper" classes around the core libraries. The main application logic (`main.cpp`) should never call a library function directly. It should interact with our custom wrapper classes.
4.  **Intelligent File Placement:** Before creating a new file, review the folder structure in the `Master_PRD.md`. Place new files in the most logical existing subdirectory.

---

## 🔧 Technical Best Practices

### 1. State Management
The device's overall behavior must be controlled by a finite state machine (FSM). The main `loop()` should consist of a `switch` statement that executes different logic based on the current `SystemState`.

### 2. Error Handling & Resilience
The device must be resilient to errors and fail gracefully.
* **Check Return Values:** Always check the return values of library functions that can fail.
* **Dedicated Error State:** If a critical, unrecoverable error occurs, the device should enter an `ERROR_STATE` and provide clear feedback to the user.

### 3. Memory Management
* **Use `PROGMEM`:** All large, constant data (especially HTML strings) **must** be stored in flash memory using `PROGMEM`.
* **String Usage:** Avoid using the `String` class in performance-critical loops. Prefer C-style character arrays (`char[]`).

### 4. Code Style & Naming Conventions
* **`const` Correctness:** Use `const` wherever possible.
* **Naming:** Use `PascalCase` for classes, `camelCase` for functions/variables, and `UPPER_SNAKE_CASE` for constants/pins.
* **Comments:** Write clear Doxygen-style comments for all public class and method headers.

### 5. Patterns to Avoid
* **No `delay()` in `loop()`:** All code must be non-blocking. Use `millis()` for timing.
* **No Global Variables:** Avoid global variables. Pass data between modules using method parameters or structs. The only exceptions are the top-level manager objects instantiated in `main.cpp`.
* **No Logic in Headers:** Do not put implementation logic in `.h` files unless it is a template class.

---

## 🔁 Phased Development Workflow

You must work on the project **one phase at a time**, following the roadmap in the `Master_PRD.md`. For each phase, the workflow is:

1.  **Review the Phase PRD:** Read the specific `PRD_PhaseX.md` file for the current phase, paying close attention to the "Module Interactions" section.
2.  **Strategize:** Briefly outline your plan as per the Golden Rule.
3.  **Implement:** Write the code, strictly following all principles in this document.
4.  **Verify Build:** After generating the code, you **must** confirm it builds successfully by stating that you have run `pio run` and it passed.