# Master Product Requirements Document: The Scout

## 1. Project Overview

This document provides the central technical specifications for "The Scout," a multi-sensor IoT device. It serves as the primary source of truth for hardware details, software architecture, and the development roadmap.

### Branding & Theme
* **Product Line:** Guardian
* **Device Name:** The Scout
* **Theme:** Medieval. This theme should influence naming conventions (e.g., device names like "The Keep", "The Scout", power supplies like "Hearthstone") and user feedback (e.g., using musical "chimes" for the buzzer instead of generic beeps).

---

## 2. Software Architecture & Principles

### Core Principle: Modularity & Reusability
All firmware development **must** be modular. Functionality should be encapsulated within separate, reusable classes. Each class or module should have a single, well-defined responsibility. This principle is paramount for creating maintainable and testable code.

### Code Quality & Best Practices
The code must be clean, well-documented, and adhere to good software engineering practices.
* **Keep Files Small:** Each file (`.h` and `.cpp`) should be focused on a single class or a small group of related functions. Avoid creating large, monolithic files that handle many different tasks.
* **Intelligent File Placement:** Before creating any new file, you **must** review the existing folder structure outlined below. Place new files in the most logical existing subdirectory. If a suitable one does not exist, create a new, logically-named subdirectory for the module.

### Library Management
Third-party libraries are managed by PlatformIO and located in the `lib/` folder. You **must not** modify the source code of these libraries. If a library's functionality needs to be extended or adapted, create a "wrapper" class within our own source directories that uses the library's public API. Do not alter the original library files.

### Core Libraries & Dependencies
To ensure consistency and leverage robust, community-tested code, this project will use the following core libraries. These **must** be used to implement their respective functionalities.

| Functionality          | Recommended Library             | PlatformIO ID                     |
|------------------------|---------------------------------|-----------------------------------|
| WiFi Setup / Portal    | WiFiManager                     | `tzapu/WiFiManager`               |
| MQTT Communication     | PubSubClient                    | `knolleary/PubSubClient`          |
| JSON Handling          | ArduinoJson                     | `bblanchon/ArduinoJson`           |
| BME280 Sensor          | Adafruit BME280 Library         | `adafruit/Adafruit BME280 Library`|
| VEML7700 Sensor        | Adafruit VEML7700 Library         | `adafruit/Adafruit VEML7700`      |
| LIS2DW12 Accelerometer | SparkFun LIS2DW12 Arduino Library | `sparkfun/SparkFun LIS2DW12 Arduino Library` |
| LD2420 mmWave Sensor   | ld2410                            | `remsh/ld2410`                    |


### Project Folder Structure
All code must adhere to this specific folder structure to ensure consistency and organization.

* **project/**
    * `platformio.ini`
    * **lib/**
    * **include/**
        * **config/**
            * `Settings.h`      // For MQTT credentials, version info, etc.
            * `Pins.h`          // For all hardware pin definitions.
        * **feedback/**
            * `FeedbackController.h` // Manages LEDs and Buzzer.
        * **network/**
            * `WifiHandler.h`   // Wrapper for WiFiManager library.
        * **setup/**
            * `DeviceManager.h` // Handles Factory Reset and other device-level setup.
        * **sensors/**
            * `SensorManager.h` // Main class to orchestrate all sensors.
            * `BME280_Sensor.h` // Wrapper for the BME280 library.
            * `...`             // Other sensor-specific wrappers.
        * **utilities/**
            * `MqttHandler.h`   // Manages all MQTT communication.
    * **src/**
        * `main.cpp`
        * **feedback/**
            * `FeedbackController.cpp`
        * **network/**
            * `WifiHandler.cpp`
        * **setup/**
            * `DeviceManager.cpp`
        * **sensors/**
            * `SensorManager.cpp`
            * `BME280_Sensor.cpp`
            * `...`
        * **utilities/**
            * `MqttHandler.cpp`

---

## 3. Hardware & Network Specifications

This section contains all fixed hardware and network configuration details.

### Pin Mapping

| Function                  | GPIO Pin | Notes                               |
|---------------------------|----------|-------------------------------------|
| Factory Reset Button      | IO02     | Input, requires pull-up             |
| System LED                | IO03     | Output, for system status           |
| Accelerometer Interrupt   | IO09     | Input, for tamper detection         |
| LD2420 Interrupt          | IO10     | Input, for presence detection       |
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
| Activity LED              | IO45     | Output, for sensor activity         |

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

The firmware for The Scout will be developed in five distinct, sequential phases. Use the corresponding PRD file for the detailed requirements of each phase.

### Phase 1: User Feedback Modules (LED & Buzzer)
* **Goal**: Create non-blocking controllers for visual and audible feedback.
* **Reference File**: `PRD_Phase1_Feedback.md`

### Phase 2: WiFi Setup & Connectivity
* **Goal**: Implement a robust, user-friendly captive portal for initial WiFi configuration.
* **Reference File**: `PRD_Phase2_WiFi.md`

### Phase 3: Factory Reset
* **Goal**: Implement a physical button trigger to erase all settings and return the device to its initial state.
* **Reference File**: `PRD_Phase3_FactoryReset.md`

### Phase 4: Sensor Integration & Data Acquisition
* **Goal**: Initialize all onboard sensors and reliably read data from them.
* **Reference File**: `PRD_Phase4_Sensors.md`

### Phase 5: MQTT Integration & Home Assistant Discovery
* **Goal**: Publish sensor data to the MQTT broker and enable auto-discovery in Home Assistant.
* **Reference File**: `PRD_Phase5_MQTT.md`

---

### Instructions for Copilot:
1.  Always refer to **this master document** for all architectural principles, core libraries, hardware specifications, and the development roadmap.
2.  Tackle the development roadmap **one phase at a time**, using the specific PRD file for each phase to guide code generation.
3.  Do not proceed to the next phase until the requirements for the current phase are met and verified.