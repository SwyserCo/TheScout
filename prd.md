# Master Product Requirements Document: The Scout

## 1. Project Overview

This document provides the central technical specifications for "The Scout," a multi-sensor IoT device. It serves as the primary source of truth for hardware details, software architecture, and the development roadmap.

### Branding & Theme
* **Product Line:** Guardian
* **Device Name:** The Scout
* **Theme:** Medieval. This theme should influence naming conventions (e.g., device names like "The Keep", "The Scout", power supplies like "Hearthstone") and user feedback (e.g., using musical "chimes" for the buzzer instead of generic beeps).

---

## 2. Software Architecture & Principles

### Core Principle: Modularity
All firmware development **must** be modular. Functionality should be encapsulated within separate classes (e.g., `Feedback`, `Network`, `Sensors`). This ensures code is reusable, testable, and easy to maintain. Avoid placing complex logic directly in the main `loop()` function.

### Library Management
Third-party libraries are managed by PlatformIO and located in the `lib/` folder. You **must not** modify the source code of these libraries. If a library's functionality needs to be extended or adapted, create a "wrapper" class within our own `src/modules` directory that uses the library's public API. Do not alter the original library files.

### Project Folder Structure
All code must adhere to the standard PlatformIO source file organization.

* **project/**
    * `platformio.ini`
    * **lib/**
    * **include/**
        * `config.h`
        * `...`
        * **modules/**
            * `Feedback.h`
            * `Network.h`
            * `Sensors.h`
            * `Mqtt.h`
    * **src/**
        * `main.cpp`
        * `...`
        * **modules/**
            * `Feedback.cpp`
            * `Network.cpp`
            * `Sensors.cpp`
            * `Mqtt.cpp`

---

## 3. Hardware & Network Specifications

This section contains all fixed hardware and network configuration details.

### Pin Mapping

| Function                  | GPIO Pin | Notes                               |
|---------------------------|----------|-------------------------------------|
| Factory Reset Button      | IO02     | Input, requires pull-up             |
| System LED                | IO09     | Output, for system status           |
| Accelerometer Interrupt   | IO10     | Input, for tamper detection         |
| LD2420 Interrupt          | IO11     | Input, for presence detection       |
| Relay Control             | IO12     | Output, to control the relay        |
| Charged Status            | IO14     | Input, from BQ24074 charge manager  |
| LD2420 RX                 | IO15     | Connects to LD2420 TX pin           |
| LD2420 TX                 | IO16     | Connects to LD2420 RX pin           |
| I2C SDA                   | IO17     | I2C Data Line                       |
| I2C SCL                   | IO18     | I2C Clock Line                      |
| USB D+                    | IO19     | For native USB (if used)            |
| USB D-                    | IO20     | For native USB (if used)            |
| Power Good                | IO21     | Input, from BQ24074 charge manager  |
| Buzzer                    | IO40     | Output, requires PWM for tones      |
| SPL Microphone            | IO41     | Analog Input (ADC)                  |
| Activity LED              | IO48     | Output, for sensor activity         |

### I2C Device Addresses

| Sensor                    | I2C Address |
|---------------------------|-------------|
| BME280 (Temp/Hum/Press)   | 0x76        |
| VEML7700 (Ambient Light)  | 0x10        |
| LIS2DW12TR (Accelerometer)| 0x19        |

### MQTT Broker Configuration

| Parameter        | Value              |
|------------------|--------------------|
| MQTT_BROKER      | "192.168.40.6"     |
| MQTT_PORT        | 1883               |
| MQTT_USER        | "mqtt-user"        |
| MQTT_PASSWORD    | "##DikTrill45"     |

---

## 4. Development Roadmap

The firmware for The Scout will be developed in four distinct, sequential phases. Use the corresponding PRD file for the detailed requirements of each phase.

### Phase 1: User Feedback Modules (LED & Buzzer)
* **Goal**: Create non-blocking controllers for visual and audible feedback.
* **Reference File**: `PRD_Phase1_Feedback.md`

### Phase 2: WiFi Setup & Connectivity
* **Goal**: Implement a robust, user-friendly captive portal for initial WiFi configuration.
* **Reference File**: `PRD_Phase2_WiFi.md`

### Phase 3: Sensor Integration & Data Acquisition
* **Goal**: Initialize all onboard sensors and reliably read data from them.
* **Reference File**: `PRD_Phase3_Sensors.md`

### Phase 4: MQTT Integration & Home Assistant Discovery
* **Goal**: Publish sensor data to the MQTT broker and enable auto-discovery in Home Assistant.
* **Reference File**: `PRD_Phase4_MQTT.md`

---

### Instructions for Copilot:
1.  Always refer to **this master document** for all architectural principles, hardware specifications, and the development roadmap.
2.  Tackle the development roadmap **one phase at a time**, using the specific PRD file for each phase to guide code generation.
3.  Do not proceed to the next phase until the requirements for the current phase are met and verified.