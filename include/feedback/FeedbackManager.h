#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "BuzzerController.h"
#include "LedController.h"

/**
 * @brief Comprehensive feedback management system for The Scout
 * 
 * Coordinates LED and buzzer feedback with persistent settings.
 * Provides high-level interface for visual and audio cues.
 */
class FeedbackManager {
public:
    /**
     * @brief Initialize feedback manager and load settings
     */
    void begin();

    /**
     * @brief Update all feedback controllers - call in main loop
     */
    void update();

    // LED Control Methods (delegated to LedController)
    /**
     * @brief Set global LED brightness
     * @param brightness 0-255 brightness level
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Start LED animation on specified pixel
     * @param pixelIndex PIXEL_SYSTEM or PIXEL_ACTIVITY
     * @param color CRGB color (FastLED format)
     * @param animation Animation type
     * @param interval Animation interval in ms
     */
    void startAnimation(uint8_t pixelIndex, CRGB color, LedAnimation animation, uint16_t interval);

    /**
     * @brief Turn off all LEDs
     */
    void turnOffLeds();

    // Buzzer Control Methods (delegated to BuzzerController)
    /**
     * @brief Play success sound
     */
    void playSuccess();

    /**
     * @brief Play failure sound
     */
    void playFailure();

    /**
     * @brief Play confirmation sound
     */
    void playConfirm();

    /**
     * @brief Play interaction sound
     */
    void playInteraction();

    // Global Settings with Persistence
    /**
     * @brief Enable/disable stealth mode (mutes all feedback)
     * @param enabled True to enable stealth mode
     */
    void setStealthMode(bool enabled);

    /**
     * @brief Get current stealth mode status
     * @return True if stealth mode is enabled
     */
    bool isStealthMode() const { return stealthMode; }

    /**
     * @brief Get current brightness setting
     * @return Current brightness (0-255)
     */
    uint8_t getBrightness() const { return currentBrightness; }

private:
    // Controller instances
    LedController ledController;
    BuzzerController buzzerController;
    
    // Settings persistence
    Preferences preferences;
    static constexpr const char* SETTINGS_NAMESPACE = "settings";
    static constexpr const char* BRIGHTNESS_KEY = "brightness";
    static constexpr const char* STEALTH_KEY = "stealth";
    
    // Current settings
    uint8_t currentBrightness = 255;
    bool stealthMode = false;
    
    /**
     * @brief Load settings from non-volatile storage
     */
    void loadSettings();
    
    /**
     * @brief Save current settings to non-volatile storage
     */
    void saveSettings();
};
