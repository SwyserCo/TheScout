# Product Requirements Document: The Scout - Phase 4 (Advanced)

## 1. Phase Goal: Implement Comprehensive Sensor Integration

The goal of this phase is to bring the sensors of the **Basic** prototype to life. This involves writing stable, non-blocking code to initialize each sensor, read data from it periodically, and implement a clear sensor fusion strategy for intelligent presence detection.

## 2. Module Interactions & Dependencies
This phase involves implementing the **`SensorManager`** and its related sensor wrapper classes. The `SensorManager` will need to:
* Interact with the **`FeedbackManager`** (defined in `PRD_Phase1_Feedback.md`) to control the Activity LED.
* Use the **`ld2410`** library for the mmWave sensor.
* Define a new data structure in a new **`DataTypes.h`** file that will be used by the **`MqttHandler`** (defined in `PRD_Phase5_MQTT.md`).

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Security & Automation
**As a homeowner**, I want to receive immediate and reliable alerts when motion is detected, and I want to know if a room is occupied even with no movement, so I can feel secure and create powerful smart home automations.

### User Story 2: Maintenance & Monitoring
**As a user**, I want to see the power, charging, and internal health status of my device in Home Assistant, so I know if the power has gone out or if the device is operating correctly.

### Acceptance Criteria

#### A. Data Structure (`include/config/DataTypes.h`)
* [ ] A new header file, `DataTypes.h`, **must** be created.
* [ ] This file must define a `struct` named `SensorData` that will serve as the data contract between the `SensorManager` and other modules. It must contain the following members:
    ```cpp
    struct SensorData {
      bool pirMotionDetected;
      bool isPresent; // The final, fused presence state
      bool isCharging;
      bool isOnBattery;
      float cpuTemperature;
      uint32_t freeHeapMemory;
    };
    ```

#### B. Sensor Fusion Logic
* [ ] The `SensorManager` **must** implement a sensor fusion strategy to provide a single, reliable `isPresent` boolean.
* [ ] **Trigger**: The `isPresent` state becomes `true` **immediately** if the **PIR sensor detects motion**.
* [ ] **Hold**: The `isPresent` state remains `true` as long as the **LD2410S continues to report presence**, even after the PIR sensor no longer detects motion.
* [ ] **Cooldown**: The `isPresent` state only becomes `false` after both the PIR has been off for a cooldown period (e.g., 30 seconds) AND the LD2410S reports no presence.

#### C. Individual Sensor Wrappers
* [ ] **`PirSensor`**: Reads the digital state of the PIR sensor on the pin defined in `HARDWARE_SPEC.md`. Must implement a 30-second "off-delay" (cooldown) to prevent rapid state changes.
* [ ] **`Ld2410sSensor`**: Wraps the `ld2410` library to read presence and motion data.
* [ ] **`PowerStatus`**: Reads the digital state of the Charge Status and Power Good pins. Provides methods like `isCharging()` and `isOnBattery()`. This class must control the Charging Status LED via the `FeedbackManager`.

#### D. Internal Metrics
* [ ] The `SensorManager` must be able to read the ESP32's internal CPU temperature.
* [ ] The `SensorManager` must be able to read the current free heap memory.

#### E. User Feedback
* [ ] The **Activity LED (`PIXEL_ACTIVITY`)** must blink with `HEARTHGUARD_BLUE` briefly every time the `SensorManager` completes a full cycle of sensor reads.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `SensorManager` orchestrates, but each sensor's logic (PIR, LD2410S, Power) is contained within its own wrapper class.
2.  **Implement Data Contract**: Create the `DataTypes.h` file and the `SensorData` struct exactly as defined. The `SensorManager` must have a public method like `getSensorData()` that returns this struct.
3.  **Implement Fusion Logic**: The core logic for combining the PIR and LD2410S signals into a single `isPresent` state is the primary task for the `SensorManager`.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. The final test is to print the contents of the `SensorData` struct to the Serial Monitor and verify that all fields are updating correctly and the `isPresent` logic behaves as described.