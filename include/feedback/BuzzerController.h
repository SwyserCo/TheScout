#pragma once
#include <Arduino.h>

/**
 * @brief Medieval-themed audio feedback controller for The Scout
 * 
 * Manages PWM-based tone generation using ESP32 LEDC peripheral.
 * Provides thematic sound cues with stealth mode capability.
 */
class BuzzerController {
public:
    /**
     * @brief Initialize the buzzer controller and play boot sound
     */
    void begin();

    /**
     * @brief Update method for non-blocking sound playback
     * Must be called in main loop
     */
    void update();

    /**
     * @brief Enable/disable stealth mode (mutes all sounds)
     * @param enabled True to enable stealth mode
     */
    void setStealthMode(bool enabled);

    /**
     * @brief Play success sound (two quick, rising notes)
     */
    void playSuccess();

    /**
     * @brief Play failure sound (single, low, longer note)
     */
    void playFailure();

    /**
     * @brief Play confirmation sound (single, sharp, medium note)
     */
    void playConfirm();

    /**
     * @brief Play interaction sound (very short, high "tick")
     */
    void playInteraction();

private:
    // Medieval-themed sound constants (frequencies in Hz)
    static constexpr uint16_t TONE_BOOT_LOW = 440;      // A4 - Boot sequence start
    static constexpr uint16_t TONE_BOOT_HIGH = 523;     // C5 - Boot sequence end
    static constexpr uint16_t TONE_SUCCESS_LOW = 523;   // C5 - Success low note
    static constexpr uint16_t TONE_SUCCESS_HIGH = 659;  // E5 - Success high note
    static constexpr uint16_t TONE_FAILURE = 220;       // A3 - Failure (low)
    static constexpr uint16_t TONE_CONFIRM = 587;       // D5 - Confirmation
    static constexpr uint16_t TONE_INTERACTION = 880;   // A5 - Quick interaction

    // Timing constants (milliseconds)
    static constexpr uint16_t DURATION_BOOT = 150;
    static constexpr uint16_t DURATION_SUCCESS_SHORT = 100;
    static constexpr uint16_t DURATION_SUCCESS_GAP = 50;
    static constexpr uint16_t DURATION_FAILURE = 300;
    static constexpr uint16_t DURATION_CONFIRM = 120;
    static constexpr uint16_t DURATION_INTERACTION = 50;

    // Sound sequence management
    enum class SoundState {
        IDLE,
        BOOT,
        SUCCESS_NOTE1,
        SUCCESS_GAP,
        SUCCESS_NOTE2,
        FAILURE,
        CONFIRM,
        INTERACTION
    };

    SoundState currentSound = SoundState::IDLE;
    unsigned long soundStartTime = 0;
    bool stealthMode = false;
    bool buzzerInitialized = false;

    // Software PWM variables
    bool isToneActive = false;
    uint16_t currentFrequency = 0;
    unsigned long halfPeriod = 0;
    unsigned long lastToggle = 0;

    /**
     * @brief Start playing a tone at specified frequency
     * @param frequency Frequency in Hz
     */
    void startTone(uint16_t frequency);

    /**
     * @brief Stop current tone
     */
    void stopTone();
};
