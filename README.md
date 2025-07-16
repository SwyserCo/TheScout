# Guardian Security System – The Scout

**The Scout** is a compact, sensor-rich device designed as part of the modular **Guardian** home security system. It focuses on presence detection and environmental monitoring, integrating seamlessly with smart home platforms like Home Assistant.

## Features

- **Sensors**: mmWave (LD2420), temperature/air quality (BME280), ambient light (VEML7700), sound (MEMS mic), accelerometer, and more.
- **Power**: USB-powered with 1800mAh battery backup (~34.6h idle, ~4.4h peak).
- **Microcontroller**: ESP32-S3-WROOM-1 with Wi-Fi connectivity.
- **User Interface**: LEDs, tactile switches, slide switch.
- **Software**: Arduino-based firmware, WiFiManager setup, OTA updates planned.
- **Integration**: Designed for Home Assistant and modular expansion.

## Development

- Custom 4-layer PCB design
- SLA resin printed enclosures
- Components sourced from LCSC
- Manufacturing by JLCPCB˚

## Pin Mapping

| Function               | GPIO Pin |
|------------------------|----------|
| Factory Reset Button   | IO02     |
| LD2420 RX              | IO15     |
| LD2420 TX              | IO16     |
| I2C SDA                | IO17     |
| I2C SCL                | IO18     |
| USB D+                 | IO19     |
| USB D-                 | IO20     |
| System LED             | IO09     |
| Accelerometer Interrupt| IO10     |
| LD2420 Interrupt       | IO11     |
| Relay                  | IO12     |
| Charged Status         | IO14     |
| Power Good             | IO21     |
| Activity LED           | IO48     |
| SPL Microphone         | IO41     |
| Buzzer                 | IO40     |

## Status

Prototyping in progress. Schematics and visuals will be added soon.
