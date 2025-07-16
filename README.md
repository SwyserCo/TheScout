# Guardian Security System – The Scout

**The Scout** is a compact, sensor-rich device designed as part of the modular **Guardian** home security system. It focuses on presence detection and environmental monitoring, integrating seamlessly with smart home platforms like Home Assistant.

## Features

### Hardware
- **Sensors**: 
  - mmWave (LD2402) for accurate presence detection
  - BME280 for temperature, humidity, and pressure
  - VEML7700 for precise light level monitoring
  - MEMS microphone for sound detection
  - Built-in tamper detection
- **Power**: USB-powered with 1800mAh battery backup (~34.6h idle, ~4.4h peak)
- **Microcontroller**: ESP32-S3-WROOM-1 with Wi-Fi connectivity
- **Interfaces**: RGB LED, Status LED, Buzzer, Reset Button

### Software
- **Connectivity**: 
  - WiFi with fallback configuration portal
  - MQTT communication
  - OTA updates support
- **Monitoring**:
  - Real-time environmental data
  - Motion detection with distance measurement
  - Sound level monitoring
  - Light level tracking
- **Security**:
  - Tamper detection alerts
  - Motion-triggered notifications
  - Sound threshold alerts
- **Integration**: 
  - Home Assistant compatible
  - MQTT-based communication
  - JSON payload format

## Development

### Hardware
- Custom 4-layer PCB design
- SLA resin printed enclosures
- Components sourced from LCSC
- Manufacturing by JLCPCB

### Software
- PlatformIO-based development environment
- Modular C++ architecture
- Full unit test coverage
- Continuous Integration/Deployment pipeline
- Automated builds and releases

### Getting Started
1. Install PlatformIO
2. Clone this repository
3. Build the project:
   ```bash
   pio run -e esp32-s3-devkitc-1
   ```

### Testing
Run native tests:
```bash
pio test -e native
```

Run embedded tests:
```bash
pio test -e test_esp32
```

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

## MQTT Integration

Base topic: `guardian/scout/<device-id>/`
- `/status` - Device online/offline status
- `/motion` - Motion detection events
- `/environment` - Temperature, humidity, pressure
- `/light` - Light level readings
- `/sound` - Sound level readings
- `/tamper` - Tamper detection events
- `/cmd` - Command topic
- `/response` - Command responses

Example payloads:
```json
// Environment data
{
    "temperature": 23.5,
    "humidity": 45.2,
    "pressure": 1013.25,
    "timestamp": 1234567890
}

// Motion detection
{
    "state": 1,
    "distance": 250,
    "timestamp": 1234567890
}
```

## Contributing
1. Fork the repository
2. Create a feature branch
3. Commit changes
4. Push to the branch
5. Open a Pull Request

## Status

- [x] Core firmware implementation
- [x] Sensor integration
- [x] MQTT communication
- [x] Unit tests
- [x] CI/CD pipeline
- [ ] OTA updates
- [ ] Battery monitoring
- [ ] Power optimization
- [ ] Production testing

[![CI/CD](https://github.com/SwyserCo/TheScout/actions/workflows/ci.yml/badge.svg)](https://github.com/SwyserCo/TheScout/actions/workflows/ci.yml)
