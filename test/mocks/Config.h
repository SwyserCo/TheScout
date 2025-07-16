#pragma once
#include <cstdint>
#include <string>

// Debug configuration
#define DEBUG 1

// Hardware Pin Definitions
namespace Config {
    // Pin Configuration
    constexpr uint8_t FACTORY_RESET_BTN = 2;    // IO02
    constexpr uint8_t SYSTEM_LED = 9;           // IO09
    constexpr uint8_t ACCEL_INT = 10;           // IO10
    constexpr uint8_t LD2420_INT = 11;          // IO11
    constexpr uint8_t RELAY_PIN = 12;           // IO12
    constexpr uint8_t CHARGED_STATUS = 14;      // IO14
    constexpr uint8_t LD2420_RX = 15;          // IO15
    constexpr uint8_t LD2420_TX = 16;          // IO16
    constexpr uint8_t STATUS_LED_PIN = 9;       // IO09
    constexpr uint8_t RGB_LED_R_PIN = 38;       // IO38
    constexpr uint8_t RGB_LED_G_PIN = 39;       // IO39
    constexpr uint8_t RGB_LED_B_PIN = 40;       // IO40
    constexpr uint8_t BUZZER_PIN = 41;          // IO41
    constexpr uint8_t TAMPER_PIN = 42;          // IO42

    // I2C Configuration
    constexpr uint8_t I2C_SDA = 17;
    constexpr uint8_t I2C_SCL = 18;
    constexpr uint32_t I2C_FREQ = 100000;

    // WiFi Configuration
    constexpr const char* DEFAULT_SSID = "Scout Setup";
    constexpr const char* DEFAULT_PASS = "setup123";
    constexpr uint16_t SETUP_PORT = 80;

    // MQTT Configuration
    constexpr const char* MQTT_SERVER = "192.168.1.100";
    constexpr uint16_t MQTT_PORT = 1883;
    constexpr uint32_t MQTT_RECONNECT_DELAY = 5000;

    // Topics
    namespace Topics {
        constexpr const char* BASE = "devices/scout/";
        constexpr const char* STATUS = "/status";
        constexpr const char* MOTION = "/motion";
        constexpr const char* ENVIRONMENT = "/environment";
        constexpr const char* LIGHT = "/light";
        constexpr const char* SOUND = "/sound";
        constexpr const char* TAMPER = "/tamper";
        constexpr const char* COMMAND = "/cmd";
        constexpr const char* RESPONSE = "/resp";
        constexpr const char* THRESHOLDS = "/thresholds";
        constexpr const char* THRESHOLDS_SET = "/thresholds/set";
        constexpr const char* ALARM = "/alarm";
    }

    // Sensor Configuration
    namespace Sensors {
        constexpr uint16_t UPDATE_INTERVAL = 1000;
        constexpr uint16_t MOTION_UPDATE_INTERVAL = 100;
        constexpr uint16_t LIGHT_UPDATE_INTERVAL = 1000;
        constexpr uint16_t TAMPER_UPDATE_INTERVAL = 100;
        constexpr uint16_t TAMPER_DEBOUNCE_MS = 50;
        constexpr uint16_t ENV_UPDATE_INTERVAL = 30000;
        constexpr uint16_t SOUND_UPDATE_INTERVAL = 100;
    }

    // MQTT Configuration
    namespace MQTT {
        namespace Defaults {
            constexpr uint16_t SOUND_THRESHOLD_SECONDS = 30;
            constexpr uint16_t TAMPER_THRESHOLD_SECONDS = 30;
        }
    }
};
