# Product Requirements Document: The Scout - Phase 5

## 1. Phase Goal: MQTT Integration & Home Assistant Discovery

The final phase is to publish sensor data to an MQTT broker and enable seamless integration with Home Assistant using its MQTT Discovery protocol.

## 2. Core Features & Logic

### User Story
**As a Home Assistant user**, I want The Scout to automatically appear in my Home Assistant instance with all its sensors, so I can easily monitor my home and create automations.

### Acceptance Criteria
1.  **MQTT Connection**:
    * The device must connect to the MQTT broker specified in the `Master_PRD.md`.
    * The `MQTT_CLIENT_ID` must be the unique `DeviceID` generated in Phase 2.
    * It must handle disconnections gracefully and attempt to reconnect.
2.  **Home Assistant Discovery**:
    * On first connect, the device must publish configuration payloads for each of its entities to the `homeassistant/` topic prefix.
3.  **Entity Publishing (The Scout - Basic)**:
    * **Device Identity**: All entities must be linked to a single "The Scout" device in Home Assistant.
    * **Binary Sensors**: The device must publish the state for the following binary sensors:
        * **PIR Motion**: `device_class: motion`
        * **LD2410S Presence**: `device_class: occupancy`
        * **LD2410S Motion**: `device_class: motion`
        * **Charging Status**: `device_class: battery_charging`
        * **Power Status**: `device_class: power` (ON if Power Good is true, OFF if on battery)
4.  **Documentation Generation**:
    * Create a separate Markdown file named `MQTT_API.md`.
    * This file must document all MQTT topics the device uses.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: Create a dedicated `MqttHandler` class in the `utilities/` directory. Its sole responsibility is managing MQTT communication.
2.  **Use Specified Libraries**: You **must** use `PubSubClient` for MQTT and `ArduinoJson` to construct the payloads.
3.  **Code Structure**: The `MqttHandler` will be called by `main.cpp`. It should accept a struct of sensor data to format and send.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. The final test is to confirm the device appears in Home Assistant with all specified entities functioning correctly.
