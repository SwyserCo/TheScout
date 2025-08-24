# Product Requirements Document: The Scout - Phase 5 (Comprehensive)

## 1. Phase Goal: Implement Full Two-Way MQTT Integration

The final phase is to connect the Scout to Home Assistant via MQTT. This involves publishing all sensor data, enabling auto-discovery, and allowing the user to control device settings (brightness, stealth mode) directly from the Home Assistant interface.

## 2. Module Interactions & Dependencies
This phase involves implementing the **`MqttHandler`** module. This module will need to:
* Get the device's connection status from the **`WifiHandler`** (defined in `PRD_Phase2_WiFi.md`).
* Get sensor data via the `SensorData` struct from the **`SensorManager`** (defined in `PRD_Phase4_Sensors.md`).
* Send commands to the **`FeedbackManager`** (defined in `PRD_Phase1_Feedback.md`) to control brightness and stealth mode.
* Use the **`PubSubClient`** and **`ArduinoJson`** libraries.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Seamless Setup
**As a user**, when I connect my Scout to my WiFi, I want it to automatically appear as a new device in my Home Assistant integration settings, so I can add it to my smart home with a single click.

### User Story 2: Intelligent Automation
**As a Home Assistant user**, I want to use the Scout's `isPresent` sensor to create an automation that turns on my hallway light but *only* when it's after sunset.

### User Story 3: Full Dashboard Control
**As a Home Assistant user**, I want to add my Scout to a dashboard and not only see its status but also control its settings, like adjusting the LED brightness or putting it into stealth mode for the night.

### Acceptance Criteria

#### A. MQTT Connection & State Logic
* [ ] **Connection Prerequisite**: The `MqttHandler` **must** only attempt to connect to the MQTT broker if the `WifiHandler` reports that it is successfully connected to WiFi.
* [ ] **Retry Logic**: If the WiFi is connected but the MQTT broker is unavailable, the device must try to reconnect to the broker every **15 seconds**.
* [ ] **User Feedback**: While in this "MQTT disconnected" state, the **System LED (`PIXEL_SYSTEM`)** must perform a **slow orange pulse** (`PULSE_SLOW` with `HEARTHGUARD_ORANGE`).

#### B. Data Publishing Strategy
* [ ] **Publish on Change**: For all binary sensors (Presence, Charging Status, Power Status), the `MqttHandler` **must** only publish a new message when the state changes. The payload **must** be the string `ON` or `OFF`.
* [ ] **Publish on Timer**: For all numeric data (CPU Temperature, Free Heap Memory), the `MqttHandler` **must** publish an update on a fixed timer, once every **60 seconds**.

#### C. Home Assistant Discovery & Entity Publishing
* [ ] **Discovery**: On its first successful connection to the MQTT broker, the device must publish the correct configuration payloads to the `homeassistant/` topic to enable auto-discovery.
* [ ] **Published Entities (Sensors)**: The device must create the following sensor entities in Home Assistant:
    * **Presence**: `binary_sensor`, `device_class: occupancy` (This is the final, fused `isPresent` state)
    * **Charging Status**: `binary_sensor`, `device_class: battery_charging`
    * **Power Status**: `binary_sensor`, `device_class: power`
    * **CPU Temperature**: `sensor`, `device_class: temperature`
    * **Free Heap Memory**: `sensor`, `device_class: data_size` (in bytes)
* [ ] **Published Entities (Controls)**: The device must create the following control entities in Home Assistant:
    * **LED Brightness**: A `number` entity (slider) that allows setting a value from 0 to 255.
    * **Stealth Mode**: A `switch` entity to toggle stealth mode on and off.

#### D. Control from Home Assistant (MQTT Commands)
* [ ] **Subscription**: The `MqttHandler` **must** subscribe to the `command_topic` for both the "LED Brightness" and "Stealth Mode" entities.
* [ ] **Error Handling**: If an invalid payload is received (e.g., "abc" for brightness), it must be safely ignored. Out-of-range numbers must be clamped (e.g., 500 becomes 255).
* [ ] **Action**: When a valid message is received, the `MqttHandler` **must** immediately call the appropriate public method on the `FeedbackManager` instance.

#### E. `MQTT_API.md` Generation
* [ ] A comprehensive `MQTT_API.md` file must be created, documenting all topics and providing example JSON payloads for both discovery and state messages.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `MqttHandler` class is solely responsible for all MQTT communication.
2.  **Decoupled Design**: The `MqttHandler`'s `publish()` method should accept the `SensorData` struct as a parameter.
3.  **Implement Two-Way Communication**: Ensure the code handles both publishing sensor data and receiving and acting upon commands.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. The final test is to confirm the device appears in Home Assistant with all entities functioning correctly.