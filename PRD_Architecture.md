# HearthGuard Scout Firmware Architecture

## 1. High-Level Overview

This document outlines the software architecture for the HearthGuard Scout firmware. The primary goal of this architecture is to create a robust, maintainable, and extensible system by enforcing a clear separation of concerns. The firmware operates as a non-blocking, state-driven application that reads sensor data, manages network connectivity, and communicates with a central server via MQTT.

---

## 2. Core Architectural Patterns

The entire firmware is built upon these foundational patterns, which are defined in detail in the `copilot-instructions.md` file:

* **Finite State Machine (FSM):** The device's overall behavior is controlled by a main state machine in `main.cpp`. This ensures predictable and sequential operation.
* **Single Responsibility Principle (SRP):** Every class has one, and only one, responsibility.
* **Hardware Abstraction (Wrapper Classes):** All direct interaction with hardware and third-party libraries is done through custom wrapper classes. This decouples the core logic from specific hardware implementations.

---

## 3. System Block Diagram

The firmware is composed of several high-level manager modules that are orchestrated by `main.cpp`.

* **`main.cpp` (Global Objects, State Machine, `setup()`, `loop()`)**
    * Calls `update()` on each of the following manager objects during every loop:
        * **`FeedbackManager`**
            * Manages `LedController`
            * Manages `BuzzerController`
            * Uses the `Preferences` Library for settings
        * **`WifiHandler`**
            * Wraps the `WiFiManager` Library
        * **`SensorManager`**
            * Manages `PirSensor`
            * Manages `Ld2410sSensor`
            * Manages `PowerStatus`
        * ...and all other top-level managers.

---

## 4. Data & Control Flow

### A. Control Flow (The "Brain")

1.  **`main.cpp` - `setup()`**:
    * The `setup()` function initializes the Serial port and then calls the `begin()` method on each of the global manager objects (`FeedbackManager`, `WifiHandler`, etc.).
    * Each manager's `begin()` method is responsible for initializing its own hardware and dependencies (e.g., `FeedbackManager` initializes the NeoPixel library, `SensorManager` initializes the sensor libraries).
    * The initial system state is set (e.g., `CONNECTING_WIFI`).

2.  **`main.cpp` - `loop()`**:
    * The `loop()` function is kept extremely clean. On every iteration, it does two things:
        1.  It calls the `update()` method on each of the global manager objects. This allows each module to handle its own non-blocking tasks (like updating LED animations or checking for sensor data).
        2.  It executes a `switch` statement based on the current `SystemState`. This is where high-level logic and state transitions occur.

### B. Data Flow (Sensor to Cloud)

This describes the journey of a single piece of sensor data (e.g., a motion event from the PIR sensor).

1.  **Hardware -> Wrapper Class:**
    * The physical PIR sensor triggers a change on its GPIO pin.
    * The `PirSensor` wrapper class, which is being called repeatedly by the `SensorManager`'s `update()` method, detects this change.

2.  **Wrapper Class -> SensorManager:**
    * The `PirSensor` object updates its internal state (e.g., `motionDetected = true`).
    * The `SensorManager` has methods like `wasMotionDetected()` that allow the main application to retrieve this state.

3.  **SensorManager -> main.cpp:**
    * In the `NORMAL_OPERATION` state, the `main.cpp` loop might periodically get the latest sensor data from the `SensorManager`.

4.  **main.cpp -> MqttHandler:**
    * The `main.cpp` function passes the collected sensor data (e.g., in a struct) to the `MqttHandler`.

5.  **MqttHandler -> Home Assistant:**
    * The `MqttHandler` takes the data, formats it into the correct JSON payload, and publishes it to the appropriate MQTT topic, sending it to Home Assistant.