# Master Product Requirements Document: The Scout

## 1. Project Overview

This document provides the central technical specifications for "The Scout," a multi-sensor IoT device. It serves as the primary source of truth for hardware details, software architecture, and the development roadmap.

### Branding & Theme
* **Product Line:** Guardian
* **Device Name:** The Scout
* **Theme:** Medieval. This theme should influence naming conventions (e.g., device names like "The Keep", "The Scout", power supplies like "Hearthstone") and user feedback (e.g., using musical "chimes" for the buzzer instead of generic beeps).

---

## 2. Software Architecture & Principles

### Core Principle: Single Responsibility Principle (SRP)
This is the most important architectural rule for this project. Every class, and by extension every module, **must** have one, and only one, reason to change. This means each class should have a single, well-defined responsibility.

### Code Quality & Best Practices
The code must be clean, well-documented, and adhere to good software engineering practices.
* **Keep Files Small:** Each file (`.h` and `.cpp`) should be focused on a single class.
* **Intelligent File Placement:** Before creating any new file, you **must** review the existing folder structure outlined below. Place new files in the most logical existing subdirectory.
* **Verbose Logging:** The firmware **must** provide clear diagnostic messages to the Serial Monitor. Key events, such as state changes in the state machine (e.g., "State changed to: WIFI_SETUP"), successful initialization of modules, or errors, must be logged. This is crucial for debugging. These logs should be wrapped in `#ifdef DEBUG` blocks to allow for a clean release build without serial output.

### Library Management
Third-party libraries are managed by PlatformIO and located in the `lib/` folder. You **must not** modify the source code of these libraries. If a library's functionality needs to be extended or adapted, create a "wrapper" class within our own source directories that uses the library's public API. Do not alter the original library files.

### Core Libraries & Dependencies
To ensure consistency and leverage robust, community-tested code, this project will use the following core libraries. These **must** be used to implement their respective functionalities.

| Functionality          | Recommended Library             | PlatformIO ID                     |
|------------------------|---------------------------------|-----------------------------------|
| WiFi Setup / Portal    | WiFiManager                     | `tzapu/WiFiManager`               |
| MQTT Communication     | PubSubClient                    | `knolleary/PubSubClient`          |
| JSON Handling          | ArduinoJson                     | `bblanchon/ArduinoJson`           |
| WS2812B LEDs           | Adafruit NeoPixel               | `adafruit/Adafruit NeoPixel`      |
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
            * `LedController.h`
            * `BuzzerController.h`
            * `FeedbackManager.h` // Orchestrates the two controllers above.
        * **network/**
            * `WifiHandler.h`   // Wrapper for WiFiManager library.
        * **setup/**
            * `DeviceManager.h` // Handles Factory Reset and other device-level setup.
        * **sensors/**
            * `SensorManager.h` // Main class to orchestrate all sensors.
            * `LD2410S_Sensor.h` // Wrapper for the BME280 library.
            * `...`             // Other sensor-specific wrappers.
        * **utilities/**
            * `MqttHandler.h`   // Manages all MQTT communication.
    * **src/**
        * `main.cpp`
        * **feedback/**
            * `LedController.cpp`
            * `BuzzerController.cpp`
            * `FeedbackManager.cpp`
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
| Factory Reset Button      | IO02     | Input, has 10k pull up resistor     |
| LED System status         | IO03     | Output, for System Status WS2812B   |
| LED System activity       | IO45     | Output, for System Activity WS2812B |
| PIR Sensor                | IO13     | Input, from PIR Sensor              |
| Charged Status            | IO14     | Input, from BQ24074 charge manager  |
| LD2420 RX                 | IO15     | Connects to LD2420 TX pin           |
| LD2420 TX                 | IO16     | Connects to LD2420 RX pin           |
| USB D+                    | IO19     | For native USB and programming      |
| USB D-                    | IO20     | For native USB and programming      |
| Power Good                | IO21     | Input, from BQ24074 charge manager  |
| Buzzer                    | IO41     | Output, requires PWM for tones      |

### MQTT Broker Configuration

| Parameter        | Value              |
|------------------|--------------------|
| MQTT_BROKER      | "192.168.40.6"     |
| MQTT_PORT        | 1883               |
| MQTT_USER        | "mqtt-user"        |
| MQTT_PASSWORD    | "##DikTrill45"     |

---

## 4. Development Roadmap

The firmware for The Scout will be developed in six distinct, sequential phases.

### Phase 0: Project Scaffolding & Core Architecture
* **Goal**: Create the complete folder structure, configuration files, and skeleton code for the entire project.
* **Reference File**: `PRD_Phase0_Scaffolding.md`

### Phase 1: User Feedback Modules (LED & Buzzer)
* **Goal**: Implement the logic for the `FeedbackManager`, `LedController`, and `BuzzerController`.
* **Reference File**: `PRD_Phase1_Feedback.md`

### Phase 2: WiFi Setup & Connectivity
* **Goal**: Implement the logic for the `WifiHandler` to manage the captive portal.
* **Reference File**: `PRD_Phase2_WiFi.md`

### Phase 3: Factory Reset
* **Goal**: Implement the logic for the `DeviceManager` to handle the factory reset button.
* **Reference File**: `PRD_Phase3_FactoryReset.md`

### Phase 4: Sensor Integration & Data Acquisition
* **Goal**: Implement the logic for the `SensorManager` and individual sensor wrappers.
* **Reference File**: `PRD_Phase4_Sensors.md`

### Phase 5: MQTT Integration & Home Assistant Discovery
* **Goal**: Implement the logic for the `MqttHandler`.
* **Reference File**: `PRD_Phase5_MQTT.md`

---

### Instructions for Copilot:
1.  Always refer to **this master document** for all architectural principles, core libraries, hardware specifications, and the development roadmap.
2.  Tackle the development roadmap **one phase at a time**, using the specific PRD file for each phase to guide code generation.
3.  Do not proceed to the next phase until the requirements for the current phase are met and verified.