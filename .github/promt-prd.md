# Product Requirements Document: MQTT Integration for The Scout

## Introduction

The Scout is a compact, sensor-rich device designed for home security and automation. This PRD defines the foundational firmware and MQTT integration required to connect The Scout to Home Assistant, enabling sensor data publishing, alarm logic, relay control, and user feedback via LEDs and buzzer. The goal is to create a plug-and-play experience for users while maintaining reliability and responsiveness.

## Goals

- Enable seamless integration with Home Assistant via MQTT auto-discovery.
- Provide real-time sensor data and alarm triggering logic.
- Support easy Wi-Fi setup using WiFiManager.
- Offer clear user feedback through LEDs and buzzer.
- Allow control of a physical relay for automating dumb devices.
- Include factory reset functionality for recovery and reconfiguration.
- Allow configurable sensor thresholds via MQTT or local configuration.
- Provide debug logging and diagnostics via MQTT or serial output.

## User Stories

### User Story 1: Wi-Fi Setup
**As a homeowner**, I want to easily connect The Scout to my Wi-Fi so I can start using it without technical knowledge.

**Acceptance Criteria:**
- Device enters AP mode if no Wi-Fi credentials are stored.
- Captive portal shows a welcome screen with a setup button.
- User enters SSID and password.
- Confirmation page is shown after submission.
- System LED flashes red during connection, green 3x on success.
- Buzzer plays chime on successful connection.

### User Story 2: MQTT Integration
**As a Home Assistant user**, I want The Scout to auto-discover and publish sensor data so I can use it in automations.

**Acceptance Criteria:**
- Device connects to MQTT broker.
- All sensors appear in Home Assistant via auto-discovery.
- Sensor data updates in real time.

### User Story 3: Alarm Logic
**As a homeowner**, I want The Scout to trigger an alarm when tampering, sound, or presence is detected while armed.

**Acceptance Criteria:**
- Device subscribes to MQTT topic for arming/disarming.
- Alarm triggers only when armed.
- Alarm state is published to MQTT.
- Activity LED flashes when alarm is triggered.

### User Story 4: Relay Control
**As a smart home enthusiast**, I want to control the relay from Home Assistant to automate dumb devices.

**Acceptance Criteria:**
- Relay appears as a switch entity in Home Assistant.
- Relay toggles within 500ms of MQTT command.

### User Story 5: Factory Reset
**As a user**, I want to reset the device to factory settings using a physical button.

**Acceptance Criteria:**
- Holding IO02 button for 5 seconds flashes red LED.
- Wi-Fi configuration is erased.
- Device reboots into setup mode.

### User Story 6: Sensor Threshold Configuration
**As an integrator**, I want to adjust sensor thresholds remotely so I can fine-tune detection sensitivity for different environments.

**Acceptance Criteria:**
- Sensor thresholds (e.g., SPL mic, LD2420) can be configured via MQTT or local config file.
- Threshold changes take effect without reflashing firmware.

### User Story 7: Debug Logging
**As a developer**, I want to receive debug logs via MQTT or serial so I can troubleshoot issues during development and deployment.

**Acceptance Criteria:**
- Device publishes debug logs to a dedicated MQTT topic or serial output.
- Logs include sensor events, connection status, and alarm triggers.

## Functional Requirements

- WiFiManager setup with captive portal and Preferences storage.
- Unique device name generation starting with `TheScout-XXXX`.
- MQTT connection and auto-discovery for all sensors.
- Sensor publishing:
  - LD2420: presence detection
  - Accelerometer: tamper detection
  - SPL microphone: sound detection
  - BME280: temperature, humidity, pressure
  - VEML7700: ambient light
  - Battery level
- Alarm logic:
  - Trigger only when armed
  - Accelerometer: movement > 3s
  - SPL mic: loud or sustained noise > 5s
  - LD2420: presence > 5s
- Relay control via MQTT.
- LED and buzzer feedback:
  - System LED: red (connecting), green 3x (connected), red (reset)
  - Activity LED: on (armed), flashing (alarm)
  - Buzzer: chimes for success, failure, timeout
- Factory reset via IO02 button.
- Configurable sensor thresholds via MQTT or local config.
- Debug logging via MQTT or serial output.

## Non-Goals

- OTA updates
- Bluetooth functionality
- Cloud-based remote access
- Encrypted MQTT communication
- Custom Home Assistant dashboards

## Design Considerations

- **Captive portal**:
  - Welcome screen with setup button
  - Responsive design for mobile
  - Confirmation page after setup
- **LED feedback**:
  - Clear status indication
- **Buzzer feedback**:
  - Predefined chimes for key events
- **Home Assistant UX**:
  - All entities clearly labeled
  - Relay exposed as toggle switch

## Technical Considerations

- ESP32-S3 is single-threaded; avoid blocking operations.
- Device is USB-powered; battery optimization not required.
- Use best-in-class libraries for MQTT, WiFiManager, Preferences.
- Sensor polling and debounce logic must be efficient.

## Success Metrics

- Setup time < 2 minutes
- MQTT discovery within 10 seconds
- Sensor accuracy ±5%
- Alarm triggers within 1 second
- Relay toggles within 500ms
- 0 false alarms during 24h idle test
- 100% feedback accuracy for LEDs and buzzer
- Sensor thresholds can be updated and verified via MQTT
- Debug logs are accessible and informative

## Open Questions

- Should alarm state be latched or auto-reset?
- Should sensor thresholds be persisted across reboots?
- Should relay support timed activation (e.g., auto-off after 5s)?
