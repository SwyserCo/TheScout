#pragma once

// Device Settings & Configuration for The Scout
// Reference: Master PRD.md Core Specifications

namespace Settings {
    // Device Information
    constexpr char DEVICE_NAME[] = "The Scout";
    constexpr char PRODUCT_LINE[] = "Guardian";
    constexpr char FIRMWARE_VERSION[] = "1.0.0";
    
    // MQTT Broker Configuration (from Master PRD)
    constexpr char MQTT_BROKER[] = "192.168.40.6";
    constexpr uint16_t MQTT_PORT = 1883;
    constexpr char MQTT_USER[] = "mqtt-user";
    constexpr char MQTT_PASSWORD[] = "##DikTrill45";
    
    // Serial Communication
    constexpr uint32_t SERIAL_BAUD_RATE = 115200;
    
    // LED Configuration
    namespace LED {
        constexpr uint8_t DEFAULT_BRIGHTNESS = 128;    // 50% brightness for power efficiency
        constexpr uint8_t MAX_BRIGHTNESS = 255;        // Full brightness
        constexpr uint8_t MIN_BRIGHTNESS = 10;         // Minimum visible brightness
        constexpr uint8_t STEALTH_BRIGHTNESS = 0;      // Completely off for stealth mode
    }
    
    // Device Operation Modes
    namespace Mode {
        constexpr bool STEALTH_MODE = false;           // Set to true to disable all LEDs/buzzer
        constexpr bool ENABLE_BUZZER = true;           // Set to false to disable buzzer sounds
        constexpr bool ENABLE_LEDS = true;             // Set to false to disable LED patterns
    }
    
    // Timing Constants (milliseconds)
    constexpr uint16_t LOOP_DELAY = 10;            // Main loop delay to prevent watchdog
    constexpr uint16_t STARTUP_DELAY = 2000;       // Initial startup delay
    constexpr uint32_t SUCCESS_TIMEOUT = 5000;     // Auto-timeout for success patterns (5 seconds)
    
    // Medieval Theme Constants
    namespace Theme {
        constexpr char STARTUP_MESSAGE[] = "=== The Scout - Guardian Device ===";
        constexpr char CHIME_DESCRIPTION[] = "Medieval Chimes";
    }
}
