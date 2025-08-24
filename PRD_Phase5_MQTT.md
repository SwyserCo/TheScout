# Product Requirements Document: The Scout - Phase 5 (Advanced)

## 1. Phase Goal: Implement Full Two-Way MQTT Integration

The final phase is to connect the Scout to Home Assistant via MQTT. This involves publishing all sensor data, enabling auto-discovery, and allowing the user to control device settings (brightness, stealth mode) directly from the Home Assistant interface.

## 2. Module Interactions & Dependencies
This phase involves implementing the **`MqttHandler`** module. This module will need to:
* Get the device's connection status from the **`WifiHandler`** (defined in `PRD_Phase2_WiFi.md`).
* Get sensor data from the **`SensorManager`** (defined in `PRD_Phase4_Sensors.md`).
* Send commands to the **`FeedbackManager`** (defined in `PRD_Phase1_Feedback.md`) to control brightness and stealth mode.
* Use the **`PubSubClient`** and **`ArduinoJson`** libraries.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Seamless Setup
**As a user**, when I connect my Scout to my WiFi, I want it to automatically appear as a new device in my Home Assistant integration settings, so I can add it to my smart home with a single click.

### User Story 2: Intelligent Automation
**As a Home Assistant user**, I want to use the Scout's presence and power status sensors to create intelligent automations, such as turning on lights when I enter a room or sending me a notification if the power goes out.

### User Story 3: Full Dashboard Control
**As a Home Assistant user**, I want to add my Scout to a dashboard and not only see its status but also control its settings, like adjusting the LED brightness or putting it into stealth mode for the night.

### Acceptance Criteria

#### A. MQTT Connection & State Logic
* [ ] **Connection Prerequisite**: The `MqttHandler` **must** only attempt to connect to the MQTT broker if the `WifiHandler` reports that it is successfully connected to WiFi.
* [ ] **Retry Logic**: If the WiFi is connected but the MQTT broker is unavailable, the device must try to reconnect to the broker every **15 seconds**.
* [ ] **User Feedback**: While in this "MQTT disconnected" state, the **System LED (`PIXEL_SYSTEM`)** must perform a **slow orange pulse** (`PULSE_SLOW` with `HEARTHGUARD_ORANGE`). This feedback must not interfere with other LED patterns (like the WiFi connection feedback).

#### B. Data Publishing Strategy
* [ ] **Publish on Change**: For all binary sensors (PIR Motion, LD2410S Presence/Motion, Charging Status, Power Status), the `MqttHandler` **must** only publish a new message when the state of the sensor changes.
* [ ] **Publish on Timer**: For all numeric data (CPU Temperature, Free Heap Memory), the `MqttHandler` **must** publish an update on a fixed timer, once every **60 seconds**.

#### C. Home Assistant Discovery & Entity Publishing
* [ ] **Discovery**: On its first successful connection to the MQTT broker, the device must publish the correct configuration payloads to the `homeassistant/` topic to enable auto-discovery.
* [ ] **Published Entities (Sensors)**: The device must create the following sensor entities in Home Assistant:
    * **PIR Motion**: `binary_sensor`, `device_class: motion`
    * **LD2410S Presence**: `binary_sensor`, `device_class: occupancy`
    * **LD2410S Motion**: `binary_sensor`, `device_class: motion`
    * **Charging Status**: `binary_sensor`, `device_class: battery_charging`
    * **Power Status**: `binary_sensor`, `device_class: power`
    * **CPU Temperature**: `sensor`, `device_class: temperature`
    * **Free Heap Memory**: `sensor`, `device_class: data_size` (in bytes)
* [ ] **Published Entities (Controls)**: The device must create the following control entities in Home Assistant:
    * **LED Brightness**: A `number` entity (slider) that allows setting a value from 0 to 255.
    * **Stealth Mode**: A `switch` entity to toggle stealth mode on and off.

#### D. Control from Home Assistant (MQTT Commands)
* [ ] **Subscription**: The `MqttHandler` **must** subscribe to the `command_topic` for both the "LED Brightness" and "Stealth Mode" entities.
* [ ] **Action**: When a message is received on these topics, the `MqttHandler` **must** immediately call the appropriate public method on the `FeedbackManager` instance (e.g., `feedbackManager.setBrightness(...)` or `feedbackManager.setStealthMode(...)`).

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `MqttHandler` class is solely responsible for all MQTT communication (connecting, subscribing, publishing). It should not contain any sensor or feedback logic itself.
2.  **Decoupled Design**: The `MqttHandler`'s `update()` or `publish()` method should accept the `SensorData` struct (defined in `PRD_Phase4_Sensors.md`) as a parameter. It reads from this struct; it does not call the `SensorManager` directly.
3.  **Implement Two-Way Communication**: Ensure the code handles both publishing sensor data and receiving and acting upon commands for brightness and stealth mode.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. The final test is to confirm the device appears in Home Assistant with all specified sensor and control entities, and that changing a control in Home Assistant (like the brightness slider) physically affects the device.
