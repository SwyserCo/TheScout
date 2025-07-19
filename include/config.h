#pragma once

// Pin Configuration (from PRD Master Document)
namespace Config {
    // LED Pins (WS2812B addressable RGB LEDs)
    constexpr uint8_t SYSTEM_LED_PIN = 3;      // IO03 - System status LED (WS2812B)
    constexpr uint8_t ACTIVITY_LED_PIN = 45;   // IO45 - Activity LED (WS2812B)
    
    // WS2812B LED Configuration
    constexpr uint8_t NUM_LEDS_PER_STRIP = 1;  // Single LED per strip
    
    // RGB Color Values (0-255)
    constexpr uint8_t LED_RED = 255;
    constexpr uint8_t LED_GREEN = 255;
    constexpr uint8_t LED_BLUE = 255;
    constexpr uint8_t LED_OFF = 0;
    
    // Buzzer Pin
    constexpr uint8_t BUZZER_PIN = 40;         // IO40 - Buzzer (PWM capable)
    
    // Serial Configuration
    constexpr uint32_t SERIAL_BAUD = 115200;
    
    // Timing Constants
    constexpr uint32_t LED_BLINK_FAST = 200;   // Fast blink interval (ms)
    constexpr uint32_t LED_BLINK_SLOW = 1000;  // Slow blink interval (ms)
    constexpr uint32_t BUZZER_BEEP_SHORT = 100; // Short beep duration (ms)
    constexpr uint32_t BUZZER_BEEP_LONG = 500;  // Long beep duration (ms)
    
    // Buzzer Frequencies
    constexpr uint16_t BUZZER_FREQ_LOW = 800;   // Low tone frequency (Hz)
    constexpr uint16_t BUZZER_FREQ_HIGH = 1200; // High tone frequency (Hz)
}