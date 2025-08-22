# Master Product Requirements Document: The Scout (Basic)

## 1. Project Overview

This document provides the central technical specifications for "The Scout," a multi-sensor IoT device. It serves as the primary source of truth for hardware details, software architecture, and the development roadmap for the **Basic** prototype.

### Branding & Theme
* **Product Line:** HearthGuard
* **Device Name:** The Scout
* **Theme:** Medieval. This theme should influence naming conventions and user feedback (e.g., using musical "chimes" for the buzzer instead of generic beeps).

---

## 2. Software Architecture & Principles

### Core Principle: Single Responsibility Principle (SRP)
This is the most important architectural rule for this project. Every class, and by extension every module, **must** have one, and only one, reason to change. This means each class should have a single, well-defined responsibility.

### Code Quality & Best Practices
The code must be clean, well-documented, and adhere to good software engineering practices.
* **Keep Files Small:** Each file (`.h` and `.cpp`) should be focused on a single class.
* **Intelligent File Placement:** Before creating any new file, you **must** review the existing folder structure outlined below. Place new files in the most logical existing subdirectory.
* **Verbose Logging:** The firmware **must** provide clear diagnostic messages to the Serial Monitor for key events like state changes, successful initializations, or errors.

### Library Management
Third-party libraries are managed by PlatformIO. You **must not** modify the source code of these libraries. If a library's functionality needs to be extended, create a "wrapper" class within our own source directories.

### Core Libraries & Dependencies
| Functionality       | Recommended Library | PlatformIO ID         |
| ------------------- | ------------------- | --------------------- |
| WiFi Setup / Portal | WiFiManager         | `tzapu/WiFiManager`   |
| MQTT Communication  | PubSubClient        | `knolleary/PubSubClient` |
| JSON Handling       | ArduinoJson         | `bblanchon/ArduinoJson` |
| WS2812B LEDs        | Adafruit NeoPixel   | `adafruit/Adafruit NeoPixel` |
| LD2410S Sensor      | ld2410              | `remsh/ld2410`        |

### Project Folder Structure
* **project/**
    * `platformio.ini`
    * **lib/**
    * **include/**
        * **config/**
            * `Settings.h`
            * `Pins.h`
        * **feedback/**
            * `LedController.h`
            * `BuzzerController.h`
            * `FeedbackManager.h`
        * **network/**
            * `WifiHandler.h`
        * **setup/**
            * `DeviceManager.h`
        * **sensors/**
            * `SensorManager.h`
            * `PirSensor.h`
            * `Ld2410sSensor.h`
            * `PowerStatus.h`
        * **utilities/**
            * `MqttHandler.h`
    * **src/**
        * `main.cpp`
        * (Matching `.cpp` files for all `.h` files above)

---

## 3. Hardware & Network Specifications

### Pin Mapping (The Scout - Basic)

| Function              | GPIO Pin | Notes                                  |
| --------------------- | -------- | -------------------------------------- |
| Factory Reset Button  | IO02     | Input, requires pull-up                |
| System LED (WS2812B)  | IO03     | Addressable LED, Pixel 0               |
| PIR Sensor Output     | IO13     | Digital Input from Logic Level Shifter |
| Charge Status Input   | IO14     | Digital Input from BQ24074             |
| LD2410S RX            | IO15     | Connects to LD2410S TX pin             |
| LD2410S TX            | IO16     | Connects to LD2410S RX pin             |
| Power Good Input      | IO21     | Digital Input from BQ24074             |
| Buzzer                | IO41     | Output, requires PWM for tones         |
| Charge Status LED     | IO42     | Digital Output, mirrors IO14 state     |
| Activity LED (WS2812B)| IO45     | Addressable LED, Pixel 1               |
| LD2410S Interrupt     | IO48     | Digital Input, for presence detection  |

### MQTT Broker Configuration
| Parameter     | Value          |
| ------------- | -------------- |
| MQTT_BROKER   | "192.168.40.6" |
| MQTT_PORT     | 1883           |
| MQTT_USER     | "mqtt-user"    |
| MQTT_PASSWORD | "##DikTrill45" |

---

## 4. Development Roadmap

### Phase 0: Project Scaffolding & Core Architecture
* **Goal**: Create the complete folder structure, configuration files, and skeleton code.
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
* **Goal**: Implement the logic for the `SensorManager` and individual sensor wrappers for PIR, LD2410S, and Power Status.
* **Reference File**: `PRD_Phase4_Sensors.md`

### Phase 5: MQTT Integration & Home Assistant Discovery
* **Goal**: Implement the logic for the `MqttHandler`.
* **Reference File**: `PRD_Phase5_MQTT.md`