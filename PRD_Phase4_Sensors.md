# Product Requirements Document: The Scout - Phase 4 (Comprehensive)

## 1. Phase Goal: Implement Comprehensive Sensor Integration

The goal of this phase is to bring the sensors of the **Basic** prototype to life. This involves writing stable, non-blocking code to initialize each sensor, read data from it periodically, and implement a clear sensor fusion strategy for intelligent presence detection.

## 2. Module Interactions & Dependencies
This phase involves implementing the **`SensorManager`** and its related sensor wrapper classes. The `SensorManager` will need to:
* Interact with the **`FeedbackManager`** (defined in `PRD_Phase1_Feedback.md`) to control the Activity LED.
* Use the **`ld2410`** library.
* Populate the **`SensorData`** struct (defined in `PRD_Phase0_Scaffolding.md`).

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Security & Automation
**As a homeowner**, I want to receive immediate and reliable alerts when motion is detected, and I want to know if a room is occupied even with no movement, so I can feel secure and create powerful smart home automations.

### User Story 2: Maintenance & Monitoring
**As a user**, I want to see the power, charging, and internal health status of my device in Home Assistant, so I know if the power has gone out or if the device is operating correctly.

### Acceptance Criteria

#### A. Data Structure (`include/config/DataTypes.h`)
* [ ] The `SensorManager` must populate the `SensorData` struct, which is the "contract" for providing data to other modules.

#### B. Sensor Fusion Logic
* [ ] The `SensorManager` **must** implement a sensor fusion strategy to provide a single, reliable `isPresent` boolean.
* [ ] **Trigger**: The `isPresent` state becomes `true` **immediately** if the **PIR sensor detects motion**.
* [ ] **Hold**: The `isPresent` state remains `true` as long as the **LD2410S continues to report presence**.
* [ ] **Cooldown**: The `isPresent` state only becomes `false` after both the PIR has been off for a 30-second cooldown period AND the LD2410S reports no presence.

#### C. Individual Sensor Wrappers & Polling
* [ ] **Polling Rate**: The main sensor `update()` loop must run on a fixed **250ms** interval.
* [ ] **`PirSensor`**: Reads the PIR sensor. Must implement a 200ms time-based debounce after any state change.
* [ ] **`Ld2410sSensor`**: Wraps the `ld2410` library to read presence and motion data.
* [ ] **`PowerStatus`**: Reads the Charge Status and Power Good pins. This can be polled less frequently, e.g., every **5 seconds**.
* [ ] **Internal Metrics**: The ESP32's internal CPU temperature and free heap memory must be read every **5 seconds**.

#### D. User Feedback
* [ ] The **Activity LED (`PIXEL_ACTIVITY`)** must blink with `HEARTHGUARD_BLUE` briefly every time the `SensorManager` completes a full cycle of sensor reads.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `SensorManager` orchestrates, but each sensor's logic is contained within its own wrapper class.
2.  **Implement Data Contract**: The `SensorManager` must have a public method like `getSensorData()` that returns the `SensorData` struct.
3.  **Implement Fusion Logic**: The core logic for combining the PIR and LD2410S signals is the primary task for the `SensorManager`.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. The final test is to print the `SensorData` struct to the Serial Monitor and verify all fields are updating correctly.