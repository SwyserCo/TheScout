# Advanced Technical Instructions for The Guardian Project

This document defines the mandatory technical rules and best practices for writing firmware for this project. Its purpose is to ensure the code is robust, maintainable, and efficient. You must adhere to these principles at all times.

## 📜 The Golden Rule: The Master PRD is Law

Before writing or changing a single line of code, you **must** refer to the `Master_PRD.md`. This document is the single source of truth for:

* **Hardware Pin Mappings**
* **Core Libraries & Dependencies**
* **Software Architecture & Folder Structure**
* **The Phased Development Roadmap**

Do not deviate from the specifications in the `Master_PRD.md`.

## 🏛️ Core Architectural Principles

1. **Modularity is Mandatory:** All functionality **must** be encapsulated in separate, reusable classes with a single, well-defined responsibility. Avoid monolithic files.
2. **Library-First Approach:** You **must** use the "Core Libraries & Dependencies" listed in the `Master_PRD.md`. Do not write custom implementations for functionality that a specified library can provide.
3. **Hardware Abstraction (Wrapper Classes):** You **must** create "wrapper" classes around the core libraries. The main application logic (`main.cpp`) should never call a library function directly. It should interact with our custom wrapper classes, which then call the library functions. This decouples our logic from specific library implementations.
   * **✅ Do:** `sensorManager.getTemperature();`
   * **🚫 Don't:** `bme.readTemperature();`
4. **Intelligent File Placement:** Before creating a new file, review the folder structure in the `Master_PRD.md`. Place new files in the most logical existing subdirectory.

## 🔧 Technical Best Practices

This section outlines specific, non-negotiable technical implementation patterns.

### 1. State Management
The device's overall behavior must be controlled by a finite state machine (FSM). This prevents unpredictable behavior and makes the logic easy to follow.

* **Implementation:** Use an `enum class` to define the system states. The main `loop()` should consist of a `switch` statement that executes different logic based on the current state.
  ```cpp
  // Example State Machine
  enum class SystemState {
    BOOTING,
    WIFI_SETUP,
    CONNECTING_WIFI,
    NORMAL_OPERATION,
    ERROR_STATE
  };

  SystemState currentState = SystemState::BOOTING;

  void loop() {
    switch (currentState) {
      case SystemState::WIFI_SETUP:
        // Run WiFiManager logic
        break;
      case SystemState::NORMAL_OPERATION:
        // Update sensors, check for MQTT messages, etc.
        break;
      // ... other cases
    }
  }

  ## 2. Error Handling & Resilience
The device must be resilient to errors and fail gracefully.

* **Check Return Values:** Always check the return values of library functions that can fail (e.g., `bme.begin()`, `mqttClient.connect()`).
* **Dedicated Error State:** If a critical, unrecoverable error occurs (e.g., a required sensor fails to initialize), the device should enter the `ERROR_STATE`. In this state, it should stop normal operations and provide clear feedback to the user (e.g., a solid red System LED).
  ```cpp
  // Example Error Handling
  void setup() {
    if (!sensorManager.begin()) {
      Serial.println("Failed to init SensorManager!");
      currentState = SystemState::ERROR_STATE;
      feedbackController.showSolidColor(SYSTEM_LED_PIN, COLOR_RED);
    }
  }

  ### 3. Memory Management
The ESP32-S3 has limited RAM. Be efficient with memory usage.

* **Use `PROGMEM`:** All large, constant data, especially the HTML/CSS/JS strings for the captive portal, **must** be stored in flash memory using `PROGMEM`.
* **String Usage:** Avoid using the `String` class in performance-critical loops, as it can cause memory fragmentation. Prefer C-style character arrays (`char[]`) for simple, fixed-size text. Use `String` where it makes sense for complex manipulation, but be mindful of its cost.
* **Stack Over Heap:** Prefer allocating objects on the stack instead of the heap (`new`).

### 4. Code Style & Naming Conventions
* **`const` Correctness:** Use `const` wherever possible for variables, method parameters, and methods that do not modify class state.
* **Naming:**
  * Classes: `PascalCase` (e.g., `SensorManager`).
  * Methods/Functions: `camelCase` (e.g., `readTemperature`).
  * Variables: `camelCase` (e.g., `currentTemp`).
  * Constants/Macros/Pins: `UPPER_SNAKE_CASE` (e.g., `SYSTEM_LED_PIN`, `MQTT_PORT`).
* **Comments:** Write clear Doxygen-style comments for all public class and method headers. Explain the *why*, not just the *what*.
  ```cpp
  /**
   * @brief Puts the device into a safe error state.
   * @param errorCode The specific error code to indicate the failure reason.
   * @note This function is blocking and will halt normal operation.
   */
  void enterErrorState(int errorCode);


  ### 3. Memory Management
The ESP32-S3 has limited RAM. Be efficient with memory usage.

* **Use `PROGMEM`:** All large, constant data, especially the HTML/CSS/JS strings for the captive portal, **must** be stored in flash memory using `PROGMEM`.
* **String Usage:** Avoid using the `String` class in performance-critical loops, as it can cause memory fragmentation. Prefer C-style character arrays (`char[]`) for simple, fixed-size text. Use `String` where it makes sense for complex manipulation, but be mindful of its cost.
* **Stack Over Heap:** Prefer allocating objects on the stack instead of the heap (`new`).

### 4. Code Style & Naming Conventions
* **`const` Correctness:** Use `const` wherever possible for variables, method parameters, and methods that do not modify class state.
* **Naming:**
  * Classes: `PascalCase` (e.g., `SensorManager`).
  * Methods/Functions: `camelCase` (e.g., `readTemperature`).
  * Variables: `camelCase` (e.g., `currentTemp`).
  * Constants/Macros/Pins: `UPPER_SNAKE_CASE` (e.g., `SYSTEM_LED_PIN`, `MQTT_PORT`).
* **Comments:** Write clear Doxygen-style comments for all public class and method headers. Explain the *why*, not just the *what*.
  ```cpp
  /**
   * @brief Puts the device into a safe error state.
   * @param errorCode The specific error code to indicate the failure reason.
   * @note This function is blocking and will halt normal operation.
   */
  void enterErrorState(int errorCode);