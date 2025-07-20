#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "config/Pins.h"
#include "config/Settings.h"

/**
 * FeedbackController - Medieval-themed WS2812B LED and Buzzer Controller
 * 
 * Manages non-blocking WS2812B LED patterns and medieval-themed buzzer chimes
 * for The Scout Guardian device. Provides visual and audible feedback
 * for system states and user interactions.
 * 
 * IMPORTANT: Must call update() in main loop for non-blocking operation!
 */
class FeedbackController {
public:
    // LED Control Methods (now using CRGB colors for WS2812B)
    void on(uint8_t pin, CRGB color = CRGB::White);     // Turn LED on solid with color
    void off(uint8_t pin);                              // Turn LED off
    void blink(uint8_t pin, uint16_t interval, CRGB color = CRGB::White);    // Blink LED at interval
    void pulse(uint8_t pin, uint16_t speed, CRGB color = CRGB::White);       // Smooth fade on/off
    
    // Predefined System LED Patterns (for future phases)
    void setSetupMode();                           // Pulsing Blue (Phase 2)
    void setConnecting();                          // Blinking Yellow (Phase 2)  
    void setSuccess();                             // Solid Green for 5s, then off (Phase 2)
    
    // Buzzer Control Methods (Medieval-themed chimes)
    void playNote(uint16_t frequency, uint16_t duration);  // Single note
    void playSuccessChime();                       // Pleasant ascending two-note chime
    void playFailureTone();                        // Low descending two-note tone
    void playResetChime();                         // Short single confirmation beep
    
    // Core System Methods
    void begin();                                  // Initialize WS2812B LEDs and PWM channels
    void update();                                 // MUST be called in main loop for non-blocking operation
    
    // Configuration Methods
    void setBrightness(uint8_t brightness);        // Set LED brightness (0-255)
    void setStealthMode(bool enabled);             // Enable/disable stealth mode (no LEDs/buzzer)
    void enableBuzzer(bool enabled);               // Enable/disable buzzer sounds
    void enableLEDs(bool enabled);                 // Enable/disable LED patterns
    
    // Status Methods
    bool isBuzzerPlaying() const;                  // Check if buzzer is active
    bool isLEDActive(uint8_t pin) const;          // Check if LED has active pattern
    bool isStealthMode() const;                    // Check if stealth mode is active

private:
    // WS2812B LED arrays
    CRGB systemLED[1];      // IO03 - Single WS2812B for system status
    CRGB activityLED[1];    // IO45 - Single WS2812B for activity
    
    // LED State Management
    struct LEDState {
        bool isActive = false;
        bool currentState = false;
        CRGB color = CRGB::Black;
        uint16_t interval = 0;
        uint16_t speed = 0;
        unsigned long lastUpdate = 0;
        uint8_t brightness = 255;
        bool fadeDirection = true;  // true = fading up, false = fading down
        enum Pattern { OFF, SOLID, BLINK, PULSE } pattern = OFF;
        unsigned long patternStartTime = 0;  // For auto-timeout patterns
        uint32_t timeoutDuration = 0;        // Pattern timeout in milliseconds
    };
    
    LEDState systemState;   // State for system LED
    LEDState activityState; // State for activity LED
    
    // Device Configuration
    uint8_t ledBrightness = Settings::LED::DEFAULT_BRIGHTNESS;
    bool stealthMode = Settings::Mode::STEALTH_MODE;
    bool buzzerEnabled = Settings::Mode::ENABLE_BUZZER;
    bool ledsEnabled = Settings::Mode::ENABLE_LEDS;
    
    // Buzzer State Management
    struct BuzzerNote {
        uint16_t frequency;
        uint16_t duration;
    };
    
    bool buzzerPlaying = false;
    unsigned long buzzerStartTime = 0;
    BuzzerNote* currentSequence = nullptr;
    uint8_t sequenceLength = 0;
    uint8_t currentNoteIndex = 0;
    unsigned long noteStartTime = 0;
    
    // PWM Configuration
    static constexpr uint8_t BUZZER_CHANNEL = 0;
    
    // Medieval Musical Notes (frequencies in Hz)
    static constexpr uint16_t NOTE_C4 = 262;
    static constexpr uint16_t NOTE_D4 = 294;
    static constexpr uint16_t NOTE_E4 = 330;
    static constexpr uint16_t NOTE_F4 = 349;
    static constexpr uint16_t NOTE_G4 = 392;
    static constexpr uint16_t NOTE_A4 = 440;
    static constexpr uint16_t NOTE_B4 = 494;
    static constexpr uint16_t NOTE_C5 = 523;
    static constexpr uint16_t NOTE_D5 = 587;
    static constexpr uint16_t NOTE_E5 = 659;
    static constexpr uint16_t NOTE_F5 = 698;
    static constexpr uint16_t NOTE_G5 = 784;
    static constexpr uint16_t NOTE_REST = 0;    // Silence
    
    // Private Helper Methods
    void updateLED(uint8_t pin, LEDState& ledState, CRGB* ledArray);
    void updateBuzzer();
    void playNoteSequence(BuzzerNote* sequence, uint8_t length);
    void stopBuzzer();
    void checkPatternTimeout(LEDState& ledState);
    LEDState& getLEDState(uint8_t pin);
};
