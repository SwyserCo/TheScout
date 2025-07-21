# Product Requirements Document: The Scout - Phase 5

## 1. Phase Goal: MQTT Integration & Home Assistant Discovery

With all device hardware and logic functioning, the final phase is to publish sensor data to an MQTT broker and enable seamless integration with Home Assistant using its MQTT Discovery protocol.

## 2. Core Features & Logic

### User Story
**As a Home Assistant user**, I want The Scout to automatically appear in my Home Assistant instance with all its sensors and controls, so I can easily monitor my home and create automations.

### Acceptance Criteria
1.  **MQTT Connection**:
    * The device must connect to the MQTT broker specified in the `Master_PRD.md`.
    * The `MQTT_CLIENT_ID` must be the unique `DeviceID` generated in Phase 2.
    * The device must handle MQTT disconnections gracefully and attempt to reconnect automatically.
2.  **Home Assistant Discovery**:
    * On first connect to the MQTT broker, the device must publish configuration payloads for each of its entities to the `homeassistant/` topic prefix. This will enable Home Assistant to automatically discover them.
    * Refer to the official [Home Assistant MQTT Discovery documentation](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery) for the correct payload structure.
3.  **Entity Publishing**:
    * **Device Identity**: All entities must be linked to a single device in Home Assistant. The device configuration payload should include identifiers like `name`, `model` ("The Scout"), and `manufacturer` ("Guardian Security").
    * **Sensor States**: The device must publish sensor data (acquired in Phase 4) to unique state topics.
        * **BME280**: Publish temperature, humidity, and pressure as separate sensor entities.
        * **VEML7700**: Publish lux value as a sensor entity.
        * **LD2420**: Publish presence as a `binary_sensor`. Payload should be `ON` for presence, `OFF` for no presence.
        * **Accelerometer (Tamper)**: Publish tamper status as a `binary_sensor` with `device_class: tamper`. It should be `ON` when the tamper flag is true, and automatically reset to `OFF` after a few seconds.
        * **Microphone (Noise)**: Publish noise status as a `binary_sensor` with `device_class: sound`. It should be `ON` when the loud noise flag is true, and automatically reset to `OFF`.
    * **Relay Control**:
        * The relay must be exposed as a `switch` entity.
        * The device must subscribe to the switch's `command_topic`.
        * It must change the relay's state immediately upon receiving a command (`ON` or `OFF`).
        * It must publish the relay's actual state to the `state_topic`.
4.  **Documentation Generation**:
    * Create a separate Markdown file named `MQTT_API.md`.
    * This file must document all MQTT topics the device uses. For each entity, it should list the discovery topic, state topic, and command topic (if applicable).

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: Create a dedicated `MqttHandler` class in the `utilities/` directory. Its sole responsibility is managing the MQTT connection, subscriptions, and publishing, as per the Single Responsibility Principle.
2.  **Use Specified Libraries**: You **must** use `PubSubClient` for MQTT and `ArduinoJson` to construct the discovery and state payloads.
3.  **Code Structure**: The `MqttHandler` will be called by `main.cpp` or other high-level managers. It should not be tightly coupled with the sensor reading logic. For example, the `SensorManager` can provide a struct of data, and the `MqttHandler` is responsible for formatting and sending it.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to confirm the device appears in Home Assistant and all entities function correctly.