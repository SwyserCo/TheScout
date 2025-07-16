#pragma once
#include <cstdint>

// Debug configuration
#ifndef DEBUG
#define DEBUG 0  // Default to debug disabled unless specified in build flags
#endif

// Hardware Pin Definitions
namespace Config {
    // Pin Configuration
    constexpr uint8_t RESET_BUTTON_PIN = 2;    // IO02 (Factory Reset button)
    constexpr uint8_t STATUS_LED_PIN = 9;      // IO09 (Also used as system LED)
    constexpr uint8_t ACCEL_INT = 10;          // IO10
    constexpr uint8_t LD2420_INT = 11;         // IO11
    constexpr uint8_t RELAY_PIN = 12;          // IO12
    constexpr uint8_t CHARGED_STATUS = 14;     // IO14
    constexpr uint8_t LD2420_RX = 15;         // IO15
    constexpr uint8_t LD2420_TX = 16;         // IO16
    constexpr uint8_t I2C_SDA_PIN = 17;       // IO17
    constexpr uint8_t I2C_SCL_PIN = 18;       // IO18
    constexpr uint8_t POWER_GOOD = 21;        // IO21
    constexpr uint8_t SPL_MIC_PIN = 41;       // IO41
    constexpr uint8_t ACTIVITY_LED = 48;      // IO48
    constexpr uint8_t BUZZER_PIN = 40;        // IO40

    // Aliases for shared pins (grouped with pin definitions for clarity)
    constexpr uint8_t SYSTEM_LED = STATUS_LED_PIN;     // IO09
    constexpr uint8_t FACTORY_RESET_BTN = RESET_BUTTON_PIN;  // IO02

    // I2C Configuration
    constexpr uint32_t I2C_FREQ = 400000;      // 400kHz

    // Sensor Addresses
    constexpr uint8_t BME280_I2C_ADDR = 0x76;
    constexpr uint8_t VEML7700_I2C_ADDR = 0x10;

    // Sensor Configuration
    constexpr uint16_t MIC_SAMPLE_RATE = 16000;
    constexpr uint8_t MIC_SAMPLES = 128;

    // Tamper Detection
    constexpr uint8_t TAMPER_PIN = 35;

    // MQTT Configuration
#ifndef MQTT_SERVER
    constexpr char MQTT_SERVER[] = "192.168.40.6";  // Default value, override in build flags
#endif
    constexpr uint16_t MQTT_PORT = 1883;
#ifndef MQTT_USER
    constexpr char MQTT_USER[] = "mqtt-user";       // Default value, override in build flags
#endif
#ifndef MQTT_PASSWORD
    constexpr char MQTT_PASSWORD[] = "##DikTrill45";    // Default value, override in build flags
#endif
    constexpr uint16_t MQTT_KEEP_ALIVE = 60;
    constexpr uint16_t MQTT_RECONNECT_DELAY = 5000;

    // Device Settings
    constexpr uint32_t DEVICE_CHECK_INTERVAL = 5000;    // 5 seconds
    constexpr uint32_t SENSOR_READ_INTERVAL = 1000;     // 1 second
    constexpr uint32_t STATUS_UPDATE_INTERVAL = 30000;  // 30 seconds
    constexpr uint32_t WATCHDOG_TIMEOUT = 30000;        // 30 seconds

    // Storage Keys
    namespace Storage {
        constexpr char DEVICE_CONFIG[] = "dev-config";
        constexpr char WIFI_CONFIG[] = "wifi-config";
        constexpr char MQTT_CONFIG[] = "mqtt-config";
        constexpr char SENSOR_CONFIG[] = "sens-config";
    }

    // Default Values
    constexpr uint16_t DEFAULT_MOTION_THRESHOLD = 50;
    constexpr uint16_t DEFAULT_SOUND_THRESHOLD = 1000;
    constexpr uint16_t DEFAULT_LIGHT_THRESHOLD = 500;
    
    // Topic Templates
    namespace Topics {
        constexpr char BASE[] = "guardian/scout/";
        constexpr char STATUS[] = "status";
        constexpr char MOTION[] = "motion";
        constexpr char ENVIRONMENT[] = "environment";
        constexpr char LIGHT[] = "light";
        constexpr char SOUND[] = "sound";
        constexpr char TAMPER[] = "tamper";
        constexpr char COMMAND[] = "cmd";
        constexpr char RESPONSE[] = "response";
        constexpr char THRESHOLDS[] = "thresholds";
        constexpr char THRESHOLDS_SET[] = "thresholds/set";
        constexpr char ALARM[] = "alarm";
    }

    // Default Thresholds
    namespace Thresholds {
        constexpr uint16_t MOTION_SECONDS = 5;
        constexpr uint16_t SOUND_SECONDS = 5;
        constexpr uint16_t TAMPER_SECONDS = 3;
        constexpr float SOUND_LEVEL = 75.0f;
    }
}
