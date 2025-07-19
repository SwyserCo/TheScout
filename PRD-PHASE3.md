# Product Requirements Document: The Scout - Phase 3

## 1. Phase Goal: Integrate with Home Assistant via MQTT

With the device connected to WiFi and sensors providing data, the final phase is to publish this information to an MQTT broker and enable seamless integration with Home Assistant using its MQTT Discovery protocol.

---

## 2. Core Features: MQTT Communication and Home Assistant Discovery

### User Story
**As a Home Assistant user**, I want The Scout to automatically appear in my Home Assistant instance with all its sensors and controls, so I can easily monitor my home and create automations.

### Acceptance Criteria
1.  **MQTT Connection**:
    * The device must connect to the MQTT broker specified in the original PRD (`192.168.40.6:1883`).
    * The `MQTT_CLIENT_ID` must be the unique `DeviceID` generated in Phase 1 (e.g., `TheScout-XXXXXX`).
    * The device must handle MQTT disconnections gracefully and attempt to reconnect automatically.
2.  **Home Assistant Discovery**:
    * On first connect to the MQTT broker, the device must publish configuration payloads for each of its entities to the `homeassistant/` topic prefix. This will enable Home Assistant to automatically discover them.
    * Refer to the official [Home Assistant MQTT Discovery documentation](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery) for the correct payload structure.
3.  **Entity Publishing**:
    * **Device Identity**: All entities must be linked to a single device in Home Assistant. The device configuration payload should include identifiers like `name`, `model` ("The Scout"), and `manufacturer` ("Guardian Security").
    * **Sensor States**: The device must publish sensor data to unique state topics.
        * **BME280**: Publish temperature, humidity, and pressure as separate sensor entities.
        * **VEML7700**: Publish lux value as a sensor entity.
        * **LD2420**: Publish presence as a `binary_sensor`. Payload should be `ON` for presence detected, `OFF` for no presence.
        * **Accelerometer (Tamper)**: Publish tamper status as a `binary_sensor` with `device_class: motion` or `device_class: tamper`. It should be `ON` when the `tamperDetected` flag is true, and automatically reset to `OFF` after a few seconds.
        * **Microphone (Noise)**: Publish noise status as a `binary_sensor` with `device_class: sound`. It should be `ON` when `loudNoiseDetected` is true, and automatically reset to `OFF`.
    * **Relay Control**:
        * The relay must be exposed as a `switch` entity.
        * The device must subscribe to the switch's `command_topic`.
        * It must change the relay's state immediately upon receiving a command (`ON` or `OFF`) from Home Assistant.
        * It must publish the relay's actual state to the `state_topic`.
4.  **Documentation Generation**:
    * Create a separate Markdown file named `MQTT_API.md`.
    * This file must document all MQTT topics the device uses. For each entity, it should list the discovery topic, state topic, and command topic (if applicable). This serves as a technical reference for debugging.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Strategize First**: Outline your plan. For example: "I will create an `MQTTHandler` class. It will manage the connection, reconnections, and publishing of discovery payloads. It will have public methods like `publishSensorData()` and `publishRelayState()` that are called from the main loop."
2.  **Library**: Use a robust MQTT client library like `PubSubClient`.
3.  **JSON Payloads**: Use a lightweight JSON library like `ArduinoJson` to construct the discovery and state payloads. Pay close attention to the exact format required by Home Assistant.
4.  **Topic Structure**: Define a clear topic structure. For example:
    * **State**: `guardian/thescout/{DeviceID}/sensor/temperature/state`
    * **Command**: `guardian/thescout/{DeviceID}/relay/command`
5.  **Verification**: After this phase, the device should successfully connect to the MQTT broker. In Home Assistant (with MQTT configured), a new device called "The Scout" should appear automatically, with all its associated sensor and switch entities. Toggling the switch in Home Assistant should physically toggle the relay.