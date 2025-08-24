#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

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
    PIXEL_SYSTEM = 0,   // System status LED
    PIXEL_ACTIVITY = 1  // Activity LED
};

/**
 * @brief HearthGuard color palette (medieval themed)
 * Using Adafruit NeoPixel Color() function for proper RGB conversion
 */
namespace HearthGuardColors {
    // These will be converted to proper colors using Color() in implementation
    struct RGBColor {
        uint8_t r, g, b;
    };
    
    constexpr RGBColor HEARTHGUARD_GREEN = {0, 255, 0};    // Pure green
    constexpr RGBColor HEARTHGUARD_RED = {255, 0, 0};      // Pure red  
    constexpr RGBColor HEARTHGUARD_BLUE = {0, 0, 255};     // Pure blue
    constexpr RGBColor HEARTHGUARD_ORANGE = {255, 165, 0}; // Orange
    constexpr RGBColor HEARTHGUARD_YELLOW = {255, 255, 0}; // Yellow
    constexpr RGBColor BLACK = {0, 0, 0};                  // Off
}

/**
 * @brief Advanced LED controller for medieval-themed IoT device
 * 
 * Manages two separate WS2812B LEDs with non-blocking animations.
 * Supports SOLID, BLINK, and PULSE animations with stealth mode.
 */
class LedController {
public:
    /**
     * @brief Constructor - initializes pointers to nullptr
     */
    LedController();

    /**
     * @brief Destructor - cleans up allocated NeoPixel objects
     */
    ~LedController();

    /**
     * @brief Initialize LED controller and hardware
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
     * @param color RGB color struct
     * @param animation Animation type
     * @param interval Animation interval in ms
     */
    void startAnimation(uint8_t pixelIndex, HearthGuardColors::RGBColor color, LedAnimation animation, uint16_t interval);

    /**
     * @brief Turn off all LEDs
     */
    void turnOff();

private:
    // Hardware configuration
    static constexpr uint8_t NUM_PIXELS = 2;
    static constexpr uint8_t FADE_STEP_MS = 50; // Fixed 50ms fade duration

    // Animation state for each pixel
    struct PixelState {
        LedAnimation animation = LedAnimation::SOLID;
        HearthGuardColors::RGBColor baseColor = {0, 0, 0};
        uint16_t interval = 0;
        unsigned long lastUpdate = 0;
        bool isOn = false;
        uint8_t pulsePhase = 0; // For sine wave pulse effect
    };

    // Hardware objects - two separate NeoPixel instances (pointers to avoid early construction)
    Adafruit_NeoPixel* systemLed;
    Adafruit_NeoPixel* activityLed;
    
    // State management
    PixelState pixelStates[NUM_PIXELS];
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
     * @param color RGB color to apply
     */
    void applyPixelColor(uint8_t pixelIndex, HearthGuardColors::RGBColor color);

    /**
     * @brief Scale color by brightness factor
     * @param color Original RGB color
     * @param brightness Brightness factor (0-255)
     * @return Scaled RGB color
     */
    HearthGuardColors::RGBColor scaleColor(HearthGuardColors::RGBColor color, uint8_t brightness);

    /**
     * @brief Convert RGB color to NeoPixel 32-bit color
     * @param color RGB color struct
     * @param pixelIndex Which pixel (for proper Color() function call)
     * @return 32-bit color value
     */
    uint32_t rgbToColor(HearthGuardColors::RGBColor color, uint8_t pixelIndex);

    /**
     * @brief Calculate sine wave value for pulse animation
     * @param phase Phase value (0-255)
     * @return Sine wave value (0-255)
     */
    uint8_t sineWave(uint8_t phase);
};
