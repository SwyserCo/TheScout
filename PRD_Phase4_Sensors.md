# Product Requirements Document: The Scout - Phase 4

## 1. Phase Goal: Sensor Integration & Data Acquisition

The goal of this phase is to bring the sensors of the **Basic** prototype to life. This involves writing stable, non-blocking code to initialize each sensor and read data from it periodically.

## 2. Core Features & Logic

### User Story
**As a developer**, I want to create a modular and efficient system to manage all of The Scout's sensors, so that I can reliably read data from them without impacting the device's responsiveness.

### Acceptance Criteria
1.  **Sensor Manager**:
    * Create a central `SensorManager` class (`sensors/SensorManager.h` & `.cpp`) responsible for initializing and orchestrating all sensor readings.
    * Its `update()` method, called in the main `loop()`, will manage polling for each sensor.
2.  **Individual Sensor Wrappers**: For each physical sensor, create a dedicated wrapper class.
3.  **PIR Sensor Integration**:
    * Create a `PirSensor` wrapper class.
    * Read the digital state of the PIR sensor on `IO13`.
    * Implement basic debouncing to avoid false triggers.
4.  **LD2410S mmWave Sensor Integration**:
    * Create a `Ld2410sSensor` wrapper class.
    * Use the `ld2410` library to read presence and motion data from the sensor on `IO15` (RX) and `IO16` (TX).
    * Monitor the interrupt pin on `IO48`.
5.  **Power Status Integration**:
    * Create a `PowerStatus` class.
    * Read the digital state of `IO14` (Charge Status) and `IO21` (Power Good).
    * Provide methods like `isCharging()` and `isOnBattery()`.
    * The `PowerStatus` class should also be responsible for controlling the simple Charging Status LED on `IO42` via the `FeedbackManager`.
6.  **User Feedback (Using Phase 1 Modules)**:
    * **Activity LED (PIXEL_ACTIVITY)**: Use the `FeedbackManager` to blink the Activity LED with `HEARTHGUARD_BLUE` briefly every time the `SensorManager` completes a full cycle of sensor reads.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `SensorManager` orchestrates, but each sensor's logic is contained within its own wrapper class.
2.  **Use Specified Libraries**: You **must** use the libraries specified in the `Master_PRD.md`.
3.  **Code Structure**: Create the `SensorManager` and individual sensor wrapper classes inside the `sensors/` directory.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. The final test is to print sensor readings to the Serial Monitor.
