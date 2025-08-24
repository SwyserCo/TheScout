#pragma once
#include <Arduino.h>
#include <FastLED.h>

/**
 * @brief LED animation types for WS2812B control
 */
enum class LedAnimation {
    SOLID,      // Static color
    BLINK,      // On/off blinking
    PULSE       // Breathing/pulsing effect
};

/**
 * @brief Pixel identifiers for The Scout
 */
enum PixelIndex {
    PIXEL_SYSTEM = 0,   // System status LED (index 0 in array)
    PIXEL_ACTIVITY = 1  // Activity LED (index 1 in array)
};

/**
 * @brief HearthGuard color palette (medieval themed)
 * Using FastLED CRGB constants as required by PRD
 */
namespace HearthGuardColors {
    const CRGB HEARTHGUARD_GREEN = CRGB::Green;
    const CRGB HEARTHGUARD_RED = CRGB::Red;  
    const CRGB HEARTHGUARD_BLUE = CRGB::Blue;
    const CRGB HEARTHGUARD_ORANGE = CRGB::OrangeRed;
    const CRGB HEARTHGUARD_YELLOW = CRGB::Yellow;
    const CRGB BLACK = CRGB::Black;
}

/**
 * @brief Advanced LED controller for medieval-themed IoT device
 * 
 * Uses FastLED library with single CRGB array and two addLeds calls
 * as specified in PRD Phase 1 requirements.
 */
class LedController {
public:
    /**
     * @brief Constructor - no hardware initialization (PRD requirement)
     */
    LedController();

    /**
     * @brief Initialize LED controller with FastLED
     */
    void begin();

    /**
     * @brief Update animations - must be called in main loop
     */
    void update();

    /**
     * @brief Set global brightness for all LEDs
     * @param brightness 0-255 brightness level
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Enable/disable stealth mode (turns off all LEDs)
     * @param enabled True to enable stealth mode
     */
    void setStealthMode(bool enabled);

    /**
     * @brief Start animation on specified pixel
     * @param pixelIndex PIXEL_SYSTEM or PIXEL_ACTIVITY
     * @param color CRGB color
     * @param animation Animation type
     * @param interval Animation interval in ms
     */
    void startAnimation(uint8_t pixelIndex, CRGB color, LedAnimation animation, uint16_t interval);

    /**
     * @brief Turn off specified LED (PRD requirement)
     * @param pixelIndex Which pixel to turn off
     */
    void turnOff(uint8_t pixelIndex);

    /**
     * @brief Turn off all LEDs
     */
    void turnOff();

private:
    // Hardware configuration (PRD: single CRGB array, two addLeds calls)
    static constexpr uint8_t LED_COUNT = 2;
    static constexpr uint8_t FADE_STEP_MS = 50; // Fixed 50ms fade duration
    
    // FastLED array for both LEDs (PRD requirement)
    CRGB leds[LED_COUNT];

    // Animation state for each pixel
    struct PixelState {
        LedAnimation animation = LedAnimation::SOLID;
        CRGB baseColor = CRGB::Black;
        uint16_t interval = 0;
        unsigned long lastUpdate = 0;
        bool isOn = false;
        uint8_t pulsePhase = 0; // For sine wave pulse effect
    };
    
    // State management
    PixelState pixelStates[LED_COUNT];
    uint8_t globalBrightness = 255;
    bool stealthMode = false;
    bool ledsInitialized = false;

    /**
     * @brief Update individual pixel animation
     * @param pixelIndex Index of pixel to update
     */
    void updatePixelAnimation(uint8_t pixelIndex);

    /**
     * @brief Apply color to specific pixel with brightness scaling
     * @param pixelIndex Which pixel to update
     * @param color CRGB color to apply
     */
    void applyPixelColor(uint8_t pixelIndex, CRGB color);

    /**
     * @brief Scale color by brightness factor
     * @param color Original CRGB color
     * @param brightness Brightness factor (0-255)
     * @return Scaled CRGB color
     */
    CRGB scaleColor(CRGB color, uint8_t brightness);

    /**
     * @brief Calculate sine wave value for pulse animation (PRD: smooth breathing)
     * @param phase Phase value (0-255)
     * @return Sine wave value (0-255)
     */
    uint8_t sineWave(uint8_t phase);
};
