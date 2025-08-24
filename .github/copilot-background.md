# HearthGuard Security System – The Scout Device (Basic Prototype)

## Introduction
This document provides background information on the **HearthGuard** security system and the specific hardware configuration of the **Basic** prototype of "The Scout" device.

HearthGuard is a modular ecosystem of smart devices that work together to provide security and automation. My goal is to first implement HearthGuard in my own home, and eventually develop it into a certified product.

## The Scout (Basic Prototype)
The Scout is a compact IoT device designed for presence and motion detection. This version is a cost-reduced prototype focused on core functionality.

### Core Components (Basic Prototype)
- **MCU**: ESP32-S3-Wroom-1(N16R8)
- **Presence Sensor**: LD2410S mmWave sensor
- **Motion Sensor**: Panasonic EKMC1601112 PIR Sensor
- **Power Management**: TI BQ24074RGTR battery charger with power path management.
- **User Feedback**:
  - Two WS2812B Addressable LEDs (System and Activity)
  - One simple LED (Charging Status)
  - One passive electromagnetic buzzer (Huaneng MLT-7525)
- **User Input**: One tactile switch for Factory Reset.

### Microcontroller and Connectivity
All HearthGuard devices are built around the **ESP32-S3**. The Scout connects to the home network via Wi-Fi and remains active at all times.

### Software and Integration
The Scout will interface with **Home Assistant**, using custom firmware developed with PlatformIO and the Arduino framework. A custom **WiFiManager** implementation is planned to simplify setup for end users.

### Pin Mapping (The Scout - Basic)
| Function              | GPIO Pin |
|-----------------------|----------|
| Factory Reset Button  | IO02     |
| System LED (WS2812B)  | IO03     |
| PIR Sensor Output     | IO13     |
| Charge Status Input   | IO14     |
| LD2410S RX            | IO15     |
| LD2410S TX            | IO16     |
| Power Good Input      | IO21     |
| Buzzer                | IO41     |
| Charge Status LED     | IO42     |
| Activity LED (WS2812B)| IO45     |
| LD2410S Interrupt     | IO48     |