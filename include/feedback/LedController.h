#pragma once

/**
 * @file LedController.h
 * @brief Controls WS2812B LED pixels for visual feedback
 */

#include <Arduino.h>
#include "config/DataTypes.h"

/**
 * @class LedController
 * @brief Manages individual WS2812B LED pixels for system status indication
 * 
 * This class provides a hardware abstraction layer for LED control,
 * supporting animations and individual pixel management.
 */
class LedController {
public:
    /**
     * @brief Constructor
     */
    LedController();

    /**
     * @brief Initialize the LED controller
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update LED animations (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to update any active LED animations.
     */
    void update();

    /**
     * @brief Set a specific pixel to a solid color
     * @param pixelIndex Index of the LED pixel (0-based)
     * @param color RGB color value (24-bit)
     */
    void setPixelColor(uint8_t pixelIndex, uint32_t color);

    /**
     * @brief Start an animation on a specific pixel
     * @param pixelIndex Index of the LED pixel
     * @param color Base color for the animation
     * @param animation Animation type to perform
     * @param interval Animation update interval in milliseconds
     */
    void startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation, uint16_t interval);

    /**
     * @brief Turn off all LEDs
     */
    void turnOffAll();

    /**
     * @brief Set global LED brightness
     * @param brightness Brightness level (0-255)
     */
    void setBrightness(uint8_t brightness);

private:
    uint8_t currentBrightness;
    unsigned long lastUpdate;
    
    // Private helper methods will be implemented in Phase 1
};
