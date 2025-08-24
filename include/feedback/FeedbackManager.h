#pragma once

/**
 * @file FeedbackManager.h
 * @brief Unified interface for all user feedback (LED and audio)
 */

#include <Arduino.h>
#include "feedback/LedController.h"
#include "feedback/BuzzerController.h"
#include "config/DataTypes.h"

/**
 * @class FeedbackManager
 * @brief Single unified interface for all user-facing feedback
 * 
 * This class coordinates LED and audio feedback to provide
 * coherent user experience and manages user preferences.
 */
class FeedbackManager {
public:
    /**
     * @brief Constructor
     */
    FeedbackManager();

    /**
     * @brief Initialize the feedback manager and all controllers
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update all feedback controllers (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to update LED animations and audio sequences.
     */
    void update();

    /**
     * @brief Set global LED brightness
     * @param brightness Brightness level (0-255)
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Enable or disable stealth mode (no feedback)
     * @param enabled True to enable stealth mode
     */
    void setStealthMode(bool enabled);

    /**
     * @brief Start an LED animation on a specific pixel
     * @param pixelIndex Index of the LED pixel
     * @param color RGB color value
     * @param animation Animation type
     * @param interval Animation interval in milliseconds
     */
    void startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation, uint16_t interval);

    /**
     * @brief Turn off all LEDs
     */
    void turnOffLeds();

    /**
     * @brief Play success feedback (LED + audio)
     */
    void playSuccess();

    /**
     * @brief Play failure feedback (LED + audio)
     */
    void playFailure();

    /**
     * @brief Play confirmation feedback (LED + audio)
     */
    void playConfirm();

    /**
     * @brief Play interaction feedback (LED + audio)
     */
    void playInteraction();

private:
    LedController ledController;
    BuzzerController buzzerController;
    uint8_t currentBrightness;
    bool stealthMode;
    
    // Private helper methods will be implemented in Phase 1
};
