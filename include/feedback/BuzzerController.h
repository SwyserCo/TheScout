#pragma once

/**
 * @file BuzzerController.h
 * @brief Controls buzzer/speaker for audio feedback
 */

#include <Arduino.h>

/**
 * @class BuzzerController
 * @brief Manages buzzer/speaker for audio feedback with medieval-themed sounds
 * 
 * This class provides audio feedback using PWM-controlled buzzer,
 * with medieval-themed sound patterns (chimes, bells, etc.).
 */
class BuzzerController {
public:
    /**
     * @brief Constructor
     */
    BuzzerController();

    /**
     * @brief Initialize the buzzer controller
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update buzzer sequences (non-blocking)
     * 
     * This method should be called regularly from the main loop
     * to handle timed audio sequences.
     */
    void update();

    /**
     * @brief Play success sound (medieval chime)
     */
    void playSuccess();

    /**
     * @brief Play failure sound (somber bell toll)
     */
    void playFailure();

    /**
     * @brief Play confirmation sound (gentle chime)
     */
    void playConfirm();

    /**
     * @brief Play interaction sound (soft bell)
     */
    void playInteraction();

    /**
     * @brief Set buzzer volume
     * @param volume Volume level (0-100)
     */
    void setVolume(uint8_t volume);

    /**
     * @brief Enable or disable stealth mode (no audio)
     * @param enabled True to enable stealth mode
     */
    void setStealthMode(bool enabled);

private:
    uint8_t currentVolume;
    bool stealthMode;
    unsigned long lastUpdate;
    
    // Private helper methods will be implemented in Phase 1
};
