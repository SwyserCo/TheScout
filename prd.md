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

## User Stories

### User Story 1: Wi-Fi Setup
**As a homeowner**, I want to easily set up The Scout for use by connecting it to my Wi-Fi so I can start using it without technical knowledge.

**Acceptance Criteria:**
- Device enters AP mode if no Wi-Fi credentials are stored in Preferences
- If user connects to AP with a phone, Captive Portal open automatically.
- User is presented with a page that show "Guardian Security System" as a heading and "The Scout" as a main heading. The page also shows the user a short 3 to 5 sentence introduction on what this page is for.
- User enters SSID and password.
- Confirmation page is shown after submission.
- The credentials are stored using the Preferences Library.
- System LED flashes red during connection, green 3x on success.
- Buzzer plays chime on successful connection.
- Once connected, create a unique DeviceID starting with "TheScout-" and then a unique 6 digit number.

### User Story 2: MQTT Integration
**As a Home Assistant user**, I want The Scout to auto-discover and publish sensor data so I can use it in automations.

**Acceptance Criteria:**
- Device connects to MQTT broker mentioned below.
- All sensors appear in Home Assistant via auto-discovery.
- Sensor data updates in real time.

### User Story 3: Alarm Logic
**As a homeowner**, I want The Scout to trigger an alarm when tampering, sound, or presence is detected while alarm is armed.

**Acceptance Criteria:**
- Device subscribes to MQTT topic for arming/disarming.
- Alarm triggers only when armed.
- Alarm state is published to MQTT.
- Activity LED flashes when alarm is triggered.
- Buzzer plays a siren chime when alarm is triggered.
- Buzzer stops when alarm is silenced, but LED keeps flashing.
- LED and Buzzer stops when alarm is disarmed.

### User Story 4: Relay Control
**As a smart home enthusiast**, I want to control the relay from Home Assistant to automate dumb devices.

**Acceptance Criteria:**
- Relay appears as a switch entity in Home Assistant.
- Relay toggles within 500ms of MQTT command.

### User Story 5: Factory Reset
**As a user**, I want to reset the device to factory settings using a physical button.

**Acceptance Criteria:**
- Holding IO02 button for 5 seconds flashes System LED and plays a short chime.
- Wi-Fi configuration is erased using PreferencesAPI.
- Device reboots into setup mode.

## Functional Requirements
- WiFiManager setup with captive portal and Preferences storage.
- Unique device name generation starting with `TheScout-` and stored useing PreferencesAPI.
- MQTT connection and auto-discovery for all sensors.
- Sensor publishing:
  - LD2420: presence detection
  - Accelerometer: tamper detection
  - SPL microphone: sound detection
  - BME280: temperature, humidity, pressure
  - VEML7700: ambient light
- Alarm logic:
  - Trigger only when armed
  - Accelerometer: movement > 3s
  - SPL mic: loud or sustained noise > 5s
  - LD2420: presence > 5s
- Relay control via MQTT.
- LED and buzzer feedback:
  - System LED: red (connecting), green 3x (connected), red (reset)
  - Activity LED: on (armed), flashing (alarm)
  - Buzzer: chimes for success, failure, timeout of wifi connection
- Factory reset when Factory Reset Button is held in for 5 seconds
  - SystemLED flashes until 5 seconds is reached
  - After 5 seconds, Wifi credentials and Unique Device Name are also cleared using PreferencesAPI
  - the buzzer plays a chime to signal the the device has been reset


## Non-Goals

- OTA updates
- Bluetooth functionality
- Cloud-based remote access
- Encrypted MQTT communication
- Custom Home Assistant dashboards
- Unit tests

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

- It's best practice to separate declarations in header files (.h) from implementations in source files (.cpp)
- Make sure you look at .github folder .md files for more insights
- ESP32-S3 is single-threaded; avoid blocking operations.
- Device is USB-powered; battery optimization not required.
- Use best-in-class libraries for MQTT, WiFiManager, Preferences.
- Sensor polling and debounce logic must be efficient.
- Update the platform.ini file to contain all the used external libraries.
- Buzzer needs PWM signal to be driven.
- Microphone is an analog MEMS microphone, it requires no clock and connects directly to an analog-to-digital converter (ADC) pin on your ESP32

| Sensor Data               | I2C address |
|---------------------------|-------------|
| BME280                    | 0x76     |
| VEML7700                  | 0x10     |
| ST LIS2DW12TR             | 0x19     |

## Success Metrics

- Sensors should be driven using Pin Map below
- Setup time < 2 minutes
- MQTT discovery within 10 seconds
- Sensor accuracy ±5%
- Alarm triggers within 1 second
- Relay toggles within 500ms
- 0 false alarms during 24h idle test
- 100% feedback accuracy for LEDs and buzzer
- Alarm state be latched
- sensor thresholds be configurable via MQTT

## Pin Mapping
- Please use the following pins.

| Function                  | GPIO Pin |
|---------------------------|----------|
| Factory Reset Button      | IO02     |
| LD2420 RX                 | IO15     |
| LD2420 TX                 | IO16     |
| I2C SDA                   | IO17     |
| I2C SCL                   | IO18     |
| USB D+                    | IO19     |
| USB D-                    | IO20     |
| System LED                | IO09     |
| Accelerometer Interrupt   | IO10     |
| LD2420 Interrupt          | IO11     |
| Relay                     | IO12     |
| Charged Status            | IO14     |
| Power Good                | IO21     |
| Activity LED              | IO48     |
| SPL Microphone            | IO41     |
| Buzzer                    | IO40     |


## MQTT Broker
- MQTT_BROKER      "192.168.40.6"
- MQTT_PORT        1883
- MQTT_USER        "mqtt-user"
- MQTT_PASSWORD    "##DikTrill45"
- MQTT_CLIENT_ID   *The Device Name that was created on wifi connect*