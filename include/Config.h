#pragma once
#include <cstdint>

// Hardware Pin Definitions
namespace Config {
    // Debug Configuration
    #define DEBUG_SERIAL 1                           // Enable debug serial output
    constexpr uint32_t SERIAL_BAUD = 115200;        // Serial baud rate
    constexpr uint32_t DEBUG_UPDATE_INTERVAL = 2000; // Debug output every 2 seconds
    
    // Pin Configuration (matching PRD pin mapping)
    constexpr uint8_t FACTORY_RESET_BTN = 2;    // IO02 (Factory Reset button)
    constexpr uint8_t SYSTEM_LED_PIN = 3;       // IO09 (System LED)
    constexpr uint8_t ACCEL_INT_PIN = 9;       // IO10 (Accelerometer Interrupt)
    constexpr uint8_t LD2420_INT_PIN = 10;      // IO11 (LD2420 Interrupt)
    constexpr uint8_t RELAY_PIN = 12;           // IO12 (Relay)
    constexpr uint8_t CHARGED_STATUS_PIN = 14;  // IO14 (Charged Status)
    constexpr uint8_t LD2420_RX_PIN = 15;       // IO15 (LD2420 RX)
    constexpr uint8_t LD2420_TX_PIN = 16;       // IO16 (LD2420 TX)
    constexpr uint8_t I2C_SDA_PIN = 17;         // IO17 (I2C SDA)
    constexpr uint8_t I2C_SCL_PIN = 18;         // IO18 (I2C SCL)
    constexpr uint8_t POWER_GOOD_PIN = 21;      // IO21 (Power Good)
    constexpr uint8_t BUZZER_PIN = 40;          // IO40 (Buzzer)
    constexpr uint8_t ACTIVITY_LED_PIN = 48;    // IO48 (Activity LED)

    // I2C Configuration
    constexpr uint32_t I2C_FREQ = 400000;      // 400kHz

    // Sensor Addresses
    constexpr uint8_t BME280_I2C_ADDR = 0x76;
    constexpr uint8_t VEML7700_I2C_ADDR = 0x10;

    // Sensor Configuration
    constexpr uint16_t MIC_SAMPLE_RATE = 16000;
    constexpr uint8_t MIC_SAMPLES = 128;

    // MQTT Configuration (from PRD)
    constexpr char MQTT_BROKER[] = "192.168.40.6";
    constexpr uint16_t MQTT_PORT = 1883;
    constexpr char MQTT_USER[] = "mqtt-user";
    constexpr char MQTT_PASSWORD[] = "##DikTrill45";
    constexpr uint16_t MQTT_KEEP_ALIVE = 60;
    constexpr uint16_t MQTT_RECONNECT_DELAY = 5000;

    // Device Settings
    constexpr uint32_t DEVICE_CHECK_INTERVAL = 1000;    // 1 second
    constexpr uint32_t SENSOR_READ_INTERVAL = 500;      // 0.5 seconds
    constexpr uint32_t STATUS_UPDATE_INTERVAL = 5000;   // 5 seconds
    constexpr uint32_t WATCHDOG_TIMEOUT = 10000;        // 10 seconds
    constexpr uint32_t WIFI_SETUP_TIMEOUT = 30000;      // 30 seconds
    
    // Network Task Settings
    constexpr uint32_t DNS_CHECK_INTERVAL = 10;         // 10ms between DNS checks
    constexpr uint32_t WEBSERVER_CHECK_INTERVAL = 10;   // 10ms between web server checks
    constexpr uint32_t NETWORK_YIELD_INTERVAL = 2;      // 2ms yield time
    constexpr uint8_t SENSOR_INIT_RETRIES = 3;         // Number of retries for sensor initialization

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
    
    // LED Settings
    constexpr uint8_t LED_DEFAULT_BRIGHTNESS = 128;     // Default LED brightness (0-255)
    constexpr uint8_t LED_DIM_BRIGHTNESS = 32;         // Dimmed LED brightness
    constexpr uint32_t LED_BLINK_INTERVAL = 500;       // Default blink interval in ms
    constexpr uint32_t LED_FAST_BLINK_INTERVAL = 200;  // Fast blink interval in ms
    constexpr uint32_t LED_PULSE_DURATION = 1000;      // Duration of one pulse cycle
    
    // Buzzer Settings
    constexpr uint32_t BUZZER_BEEP_DURATION = 100;     // Duration of a single beep
    constexpr uint32_t BUZZER_ALARM_INTERVAL = 1000;   // Interval between alarm sounds
    constexpr uint32_t BUZZER_PATTERN_PAUSE = 500;     // Pause between pattern repetitions
    
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
