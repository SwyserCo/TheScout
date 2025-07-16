#pragma once
#include "../Config.h"
#include <functional>
#include <Arduino.h>

class AlarmController {
public:
    // States
    enum class State {
        DISARMED,
        ARMED,
        TRIGGERED
    };

    // Triggers
    enum class Trigger {
        NONE,
        MOTION,
        SOUND,
        TAMPER
    };

    // Initialize the controller
    void begin();
    void update();

    // State management
    void arm();
    void disarm();
    bool isArmed() const;
    bool isTriggered() const;
    Trigger getTriggerSource() const;

    // Thresholds
    void setMotionThreshold(uint16_t seconds);
    void setSoundThreshold(uint16_t seconds);
    void setTamperThreshold(uint16_t seconds);
    void setSoundLevelThreshold(float level);
    
    uint16_t getMotionThreshold() const { return motionThreshold / 1000; }
    uint16_t getSoundThreshold() const { return soundThreshold / 1000; }
    uint16_t getTamperThreshold() const { return tamperThreshold / 1000; }
    float getSoundLevelThreshold() const { return soundLevelThreshold; }

    // Callbacks for sensor events
    void onMotionDetected(bool detected);
    void onSoundDetected(float level);
    void onTamperDetected(bool tampered);

    // State change callback
    using StateCallback = std::function<void(State, Trigger)>;
    void setStateCallback(StateCallback callback) { stateCallback = callback; }

private:
    State currentState = State::DISARMED;
    Trigger currentTrigger = Trigger::NONE;

    // Event timers
    unsigned long motionStartTime = 0;
    unsigned long soundStartTime = 0;
    unsigned long tamperStartTime = 0;

    // Current thresholds in milliseconds
    uint16_t motionThreshold = 5000;  // 5 seconds default
    uint16_t soundThreshold = 5000;   // 5 seconds default
    uint16_t tamperThreshold = 3000;  // 3 seconds default
    float soundLevelThreshold = 80.0F; // 80dB default

    // Current sensor states
    bool motionDetected = false;
    bool soundDetected = false;
    bool tamperDetected = false;

    StateCallback stateCallback;

    void checkTriggers();
    void setState(State newState, Trigger trigger);
};
