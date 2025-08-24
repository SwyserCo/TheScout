# Hardware Specification for The Scout (Basic Prototype)

This document defines the exact pin assignments and hardware interfaces for the HearthGuard Scout Basic prototype. This file is the single source of truth for all hardware-related definitions.

## Pin Assignments

| Function              | GPIO Pin | Notes                                                              |
| --------------------- | :------: | ------------------------------------------------------------------ |
| Factory Reset Button  |   IO02   | Input, requires external pull-up resistor. Active low.             |
| System LED (WS2812B)  |   IO03   | Data pin for the System addressable LED.                           |
| PIR Sensor Output     |   IO13   | Digital Input from Logic Level Shifter.                            |
| Charge Status Input   |   IO14   | Digital Input from BQ24074. HIGH = Charging, LOW = Charge Complete. |
| LD2410S RX            |   IO15   | Connects to LD2410S TX pin.                                        |
| LD2410S TX            |   IO16   | Connects to LD2410S RX pin.                                        |
| Power Good Input      |   IO21   | Digital Input from BQ24074. HIGH = External Power, LOW = On Battery. |
| Buzzer                |   IO41   | Output for passive buzzer. Requires PWM for tone generation.       |
| Charge Status LED     |   IO42   | Simple Digital Output, mirrors the state of IO14.                  |
| Activity LED (WS2812B)|   IO45   | Data pin for the Activity addressable LED.                         |
| LD2410S Interrupt     |   IO48   | Digital Input, for presence/motion detection events.               |

## Serial Devices

| Device  | Interface | Pins     | Baud Rate |
|---------|-----------|----------|-----------|
| LD2410S | UART      | 15, 16   | 256000    |

## Notes

- All pins are ESP32-S3 GPIO pins.
- The two WS2812B LEDs are on separate data pins and must be controlled as two separate instances.
- The project does not use the I2C bus on this hardware version.