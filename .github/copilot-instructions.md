# GitHub Copilot Instructions for Arduino Projects (PlatformIO + VSCode)

These instructions define how GitHub Copilot should assist with this project. The goal is to ensure consistent, high-quality code generation aligned with Arduino development practices using PlatformIO in VSCode.

---

## 🧠 Context

- **Project Type**: IoT / Embedded Systems
- **Language**: C++ (Arduino Framework)
- **Framework**: Arduino
- **Toolchain**: PlatformIO with VSCode
- **Target MCU**: ESP32-S3

---

## 🔧 General Guidelines

- Use Arduino-style C++ idioms and avoid STL features not supported on embedded platforms.
- Use `const`, `constexpr`, and `#define` for constants where appropriate.
- Avoid dynamic memory allocation (`new`, `malloc`) unless absolutely necessary.
- Use non-blocking code patterns (e.g., `millis()` instead of `delay()`).
- Keep `loop()` responsive; avoid long blocking operations.
- Use `Preferences` or `EEPROM` for persistent storage.
- Use `Serial.print()` for debugging and diagnostics.
- Follow PlatformIO's `src/` and `include/` directory structure.
- Prefer modular code with clear separation of concerns (e.g., sensor drivers, communication logic, UI feedback).
- Use `enum class` for state machines and mode definitions.

---

## 📁 File Structure

Use this structure as a guide when creating or updating files:

src/
    main.cpp
    sensors/
        SensorManager.cpp
        BME280.cpp
        VEML7700.cpp
        LD2402.cpp
        Microphone.ccp
        Relay.cpp
    utility
        MQTTHandler.cpp
        LEDController.cpp
        Buzzer.cpp
        LEDController.cpp
        TamperingProtection.cpp
    setup
        WiFiManager.cpp
        FactoryResetHandler.cpp
config/
    Config.h
include/
    sensors/
        SensorManager.h
        BME280.h
        VEML7700.h
        LD2402.h
        Microphone.h
        Relay.h
    utility
        MQTTHandler.h
        LEDController.h
        Buzzer.h
        LEDController.h
        TamperingProtection.h
    setup
        WiFiManager.h
        FactoryResetHandler.h
third_party_libs/
test/
unit/
integration/

---

## 🧶 Patterns

### ✅ Patterns to Follow

- Use classes to encapsulate hardware components and logic.
- Use `setup()` for initialization and `loop()` for non-blocking execution.
- Use `enum class` for device states and modes.
- Use `Serial.print()` for runtime diagnostics.
- Use `Preferences` for storing Wi-Fi credentials and device settings.
- Use `millis()`-based timing for periodic tasks.
- Use `#ifdef DEBUG` blocks for conditional logging.
- Use `#pragma once` where needed.

---

### 🚫 Patterns to Avoid

- Avoid raw pointers and manual memory management.
- Avoid blocking calls like `delay()` in `loop()`.
- Avoid global variables unless scoped and documented.
- Avoid putting logic in header files unless templated.
- Avoid STL containers unless verified to work on target MCU.

---

## 🧪 Testing Guidelines

- Use PlatformIO’s `test/` folder for unit and integration tests.
- Use mocks or stubs for hardware interfaces.
- Test logic in isolation from hardware dependencies.
- Use assertions to validate expected behavior.
- Simulate sensor input and verify system response.

---

## 🧩 Example Prompts

- `Copilot, create a class called SensorManager that reads from BME280 and publishes to MQTT.`
- `Copilot, write a non-blocking LED controller using millis().`
- `Copilot, implement a WiFiManager class that uses Preferences to store credentials.`
- `Copilot, write a unit test for the AlarmController class using PlatformIO's test framework.`
- `Copilot, create a function that triggers a buzzer pattern when Wi-Fi connects.`

---

## 🔁 Iteration & Review

- Always review Copilot-generated code for correctness and responsiveness.
- Use comments to guide Copilot and explain hardware-specific logic.
- Refactor output to match project structure and naming conventions.
- Run PlatformIO build and test tasks to validate generated code.

---

## 📚 References

- Arduino Language Reference
- PlatformIO Documentation
- [ESP32 Arduino Core](https://github.com/espressif/ardu) Practices
