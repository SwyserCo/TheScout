# Pin Mapping for The Scout

This document defines the exact pin assignments for The Scout hardware.

## Pin Assignments

| Function                  | GPIO Pin | Notes                           |
|---------------------------|----------|---------------------------------|
| Factory Reset Button      | IO02     | Internal pullup, active low     |
| System LED                | IO09     | Status indication               |
| Accelerometer Interrupt   | IO10     | MPU6050 interrupt pin          |
| LD2420 Interrupt          | IO11     | Presence sensor interrupt      |
| Relay                     | IO12     | Control relay output           |
| Charged Status            | IO14     | Battery charging status        |
| LD2420 RX                 | IO15     | Serial communication           |
| LD2420 TX                 | IO16     | Serial communication           |
| I2C SDA                   | IO17     | I2C data line                  |
| I2C SCL                   | IO18     | I2C clock line                 |
| USB D+                    | IO19     | USB data positive              |
| USB D-                    | IO20     | USB data negative              |
| Power Good                | IO21     | Power supply status            |
| Buzzer                    | IO40     | Audio output                   |
| Activity LED              | IO48     | Alarm status indication        |

## I2C Devices

| Device    | Address | Function                    |
|-----------|---------|----------------------------|
| BME280    | 0x76    | Temperature/Humidity/Pressure |
| VEML7700  | 0x10    | Ambient light sensor       |
| MPU6050   | 0x68    | Accelerometer/Gyroscope    |

## Serial Devices

| Device  | Interface | Pins     | Baud Rate |
|---------|-----------|----------|-----------|
| LD2420  | UART1     | 15,16    | 256000    |

## Notes

- All pins are ESP32-S3 GPIO pins
- Internal pullups are enabled where appropriate
- I2C frequency is set to 400kHz
- ADC resolution is 12-bit for analog sensors