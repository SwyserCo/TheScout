#pragma once

// Pin Configuration (from PRD Master Document)
namespace Config {
    // LED Pins (WS2812B addressable RGB LEDs)
    constexpr uint8_t SYSTEM_LED_PIN = 3;      // IO03 - System status LED (WS2812B)
    constexpr uint8_t ACTIVITY_LED_PIN = 45;   // IO45 - Activity LED (WS2812B)
    
    // WS2812B LED Configuration
    constexpr uint8_t NUM_LEDS_PER_STRIP = 1;  // Single LED per strip
    constexpr uint8_t LED_BRIGHTNESS = 128;    // Default brightness (0-255)
    
    // RGB Color Values (0-255) - Medieval themed colors
    constexpr uint8_t LED_RED = 255;
    constexpr uint8_t LED_GREEN = 255;
    constexpr uint8_t LED_BLUE = 255;
    constexpr uint8_t LED_OFF = 0;
    
    // Medieval-themed color presets
    namespace Colors {
        constexpr CRGB TORCH_ORANGE = CRGB(255, 140, 0);    // Warm torch light
        constexpr CRGB ROYAL_PURPLE = CRGB(102, 51, 153);   // Noble purple
        constexpr CRGB CASTLE_BLUE = CRGB(70, 130, 180);    // Steel blue
        constexpr CRGB DANGER_RED = CRGB(220, 20, 60);      // Alert red
        constexpr CRGB SUCCESS_GREEN = CRGB(50, 205, 50);   // Victory green
    }
    
    // Buzzer Pin
    constexpr uint8_t BUZZER_PIN = 40;         // IO40 - Buzzer (PWM capable)
    
    // Serial Configuration
    constexpr uint32_t SERIAL_BAUD = 115200;
    
    // System Configuration
    constexpr uint32_t LOOP_DELAY_MS = 10;     // Small delay to prevent watchdog issues
    constexpr uint32_t STARTUP_DELAY_MS = 2000; // Delay before starting main operations
    
    // Timing Constants
    constexpr uint32_t LED_BLINK_FAST = 200;   // Fast blink interval (ms)
    constexpr uint32_t LED_BLINK_SLOW = 1000;  // Slow blink interval (ms)
    constexpr uint32_t BUZZER_BEEP_SHORT = 100; // Short beep duration (ms)
    constexpr uint32_t BUZZER_BEEP_LONG = 500;  // Long beep duration (ms)
    
    // Buzzer Frequencies
    constexpr uint16_t BUZZER_FREQ_LOW = 800;   // Low tone frequency (Hz)
    constexpr uint16_t BUZZER_FREQ_HIGH = 1200; // High tone frequency (Hz)
    
    // Musical Notes (Hz) - Extended for medieval themes
    constexpr uint16_t NOTE_C3 = 131;   // Lower octave for bell tones
    constexpr uint16_t NOTE_D3 = 147;
    constexpr uint16_t NOTE_F3 = 175;
    constexpr uint16_t NOTE_G3 = 196;
    constexpr uint16_t NOTE_A3 = 220;
    constexpr uint16_t NOTE_C4 = 262;
    constexpr uint16_t NOTE_D4 = 294;
    constexpr uint16_t NOTE_E4 = 330;
    constexpr uint16_t NOTE_F4 = 349;
    constexpr uint16_t NOTE_G4 = 392;
    constexpr uint16_t NOTE_A4 = 440;
    constexpr uint16_t NOTE_B4 = 494;
    constexpr uint16_t NOTE_C5 = 523;
    constexpr uint16_t NOTE_D5 = 587;
    constexpr uint16_t NOTE_REST = 0;  // Silence
    
    // Note durations (ms) - Faster medieval timing
    constexpr uint16_t NOTE_QUARTER = 150;     // Faster base tempo
    constexpr uint16_t NOTE_EIGHTH = 75;       // Quick notes
    constexpr uint16_t NOTE_HALF = 300;        // Medium sustain
    constexpr uint16_t NOTE_WHOLE = 600;       // Bell tolls
    constexpr uint16_t NOTE_DOTTED_HALF = 450; // Medieval timing
    constexpr uint16_t NOTE_SIXTEENTH = 50;    // Very quick ornaments
}