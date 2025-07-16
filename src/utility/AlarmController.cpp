#include "utility/AlarmController.h"

void AlarmController::begin() {
    currentState = State::DISARMED;
    currentTrigger = Trigger::NONE;
}

void AlarmController::update() {
    if (currentState == State::ARMED) {
        checkTriggers();
    }
}

void AlarmController::arm() {
    setState(State::ARMED, Trigger::NONE);
}

void AlarmController::disarm() {
    setState(State::DISARMED, Trigger::NONE);
    // Reset all timers
    motionStartTime = 0;
    soundStartTime = 0;
    tamperStartTime = 0;
}

bool AlarmController::isArmed() const {
    return currentState == State::ARMED;
}

bool AlarmController::isTriggered() const {
    return currentState == State::TRIGGERED;
}

AlarmController::Trigger AlarmController::getTriggerSource() const {
    return currentTrigger;
}

void AlarmController::setMotionThreshold(uint16_t seconds) {
    motionThreshold = seconds * 1000;
}

void AlarmController::setSoundThreshold(uint16_t seconds) {
    soundThreshold = seconds * 1000;
}

void AlarmController::setTamperThreshold(uint16_t seconds) {
    tamperThreshold = seconds * 1000;
}

void AlarmController::setSoundLevelThreshold(float level) {
    soundLevelThreshold = level;
}

void AlarmController::onMotionDetected(bool detected) {
    motionDetected = detected;
    if (detected && currentState == State::ARMED) {
        if (motionStartTime == 0) {
            motionStartTime = millis();
        }
    } else {
        motionStartTime = 0;
    }
}

void AlarmController::onSoundDetected(float level) {
    soundDetected = level > Config::DEFAULT_SOUND_THRESHOLD;
    if (soundDetected && currentState == State::ARMED) {
        if (soundStartTime == 0) {
            soundStartTime = millis();
        }
    } else {
        soundStartTime = 0;
    }
}

void AlarmController::onTamperDetected(bool tampered) {
    tamperDetected = tampered;
    if (tampered && currentState == State::ARMED) {
        if (tamperStartTime == 0) {
            tamperStartTime = millis();
        }
    } else {
        tamperStartTime = 0;
    }
}

void AlarmController::checkTriggers() {
    unsigned long currentTime = millis();

    // Check motion trigger
    if (motionDetected && motionStartTime > 0) {
        if (currentTime - motionStartTime >= motionThreshold) {
            setState(State::TRIGGERED, Trigger::MOTION);
            return;
        }
    }

    // Check sound trigger
    if (soundDetected && soundStartTime > 0) {
        if (currentTime - soundStartTime >= soundThreshold) {
            setState(State::TRIGGERED, Trigger::SOUND);
            return;
        }
    }

    // Check tamper trigger
    if (tamperDetected && tamperStartTime > 0) {
        if (currentTime - tamperStartTime >= tamperThreshold) {
            setState(State::TRIGGERED, Trigger::TAMPER);
            return;
        }
    }
}

void AlarmController::setState(State newState, Trigger trigger) {
    if (currentState != newState) {
        currentState = newState;
        currentTrigger = trigger;
        if (stateCallback) {
            stateCallback(newState, trigger);
        }
    }
}
