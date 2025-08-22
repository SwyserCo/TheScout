# Advanced Technical Instructions for The HearthGuard Project

This document defines the mandatory technical rules and best practices for writing firmware for this project. Its purpose is to ensure the code is robust, maintainable, and efficient. You must adhere to these principles at all times.

## 📜 The Golden Rule: The Master PRD is Law

Before writing or changing a single line of code, you **must** refer to the `Master_PRD.md`. This document is the single source of truth for:

* **Hardware Pin Mappings**
* **Core Libraries & Dependencies**
* **Software Architecture & Folder Structure**
* **The Phased Development Roadmap**

Do not deviate from the specifications in the `Master_PRD.md`.

## 🏛️ Core Architectural Principles

1.  **Modularity is Mandatory:** All functionality **must** be encapsulated in separate, reusable classes with a single, well-defined responsibility. Avoid monolithic files.
2.  **Library-First Approach:** You **must** use the "Core Libraries & Dependencies" listed in the `Master_PRD.md`. Do not write custom implementations for functionality that a specified library can provide.
3.  **Hardware Abstraction (Wrapper Classes):** You **must** create "wrapper" classes around the core libraries. The main application logic (`main.cpp`) should never call a library function directly. It should interact with our custom wrapper classes, which then call the library functions. This decouples our logic from specific library implementations.
4.  **Intelligent File Placement:** Before creating a new file, review the folder structure in the `Master_PRD.md`. Place new files in the most logical existing subdirectory.

## 🔧 Technical Best Practices

### 1. State Management
The device's overall behavior must be controlled by a finite state machine (FSM). This prevents unpredictable behavior and makes the logic easy to follow.

* **Implementation:** Use an `enum class` to define the system states. The main `loop()` should consist of a `switch` statement that executes different logic based on the current state.

### 2. Error Handling & Resilience
The device must be resilient to errors and fail gracefully.

* **Check Return Values:** Always check the return values of library functions that can fail (e.g., `sensor.begin()`, `mqttClient.connect()`).
* **Dedicated Error State:** If a critical, unrecoverable error occurs, the device should enter an `ERROR_STATE` and provide clear feedback to the user (e.g., a solid red System LED).

### 3. Memory Management
* **Use `PROGMEM`:** All large, constant data, especially the HTML/CSS/JS strings for the captive portal, **must** be stored in flash memory using `PROGMEM`.
* **String Usage:** Avoid using the `String` class in performance-critical loops. Prefer C-style character arrays (`char[]`).

### 4. Code Style & Naming Conventions
* **`const` Correctness:** Use `const` wherever possible.
* **Naming:**
    * Classes: `PascalCase` (e.g., `SensorManager`).
    * Methods/Functions: `camelCase` (e.g., `readTemperature`).
    * Variables: `camelCase` (e.g., `currentTemp`).
    * Constants/Macros/Pins: `UPPER_SNAKE_CASE` (e.g., `SYSTEM_LED_PIN`).
* **Comments:** Write clear Doxygen-style comments for all public class and method headers.

## 🔁 Phased Development Workflow

You must work on the project **one phase at a time**, following the roadmap in the `Master_PRD.md`. For each phase, the workflow is:

1.  **Review the Phase PRD:** Read the specific `PRD_PhaseX.md` file for the current phase.
2.  **Strategize:** Before writing code, briefly outline your plan.
3.  **Implement:** Write the code, strictly following all principles in this document.
4.  **Verify Build:** After generating the code, you **must** confirm it builds successfully by stating that you have run `pio run` and it passed.