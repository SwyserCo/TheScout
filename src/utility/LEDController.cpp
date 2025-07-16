#include "utility/LEDController.h"
#include <Arduino.h>

void LEDController::begin() {
    pinMode(systemLEDPin, OUTPUT);
    pinMode(activityLEDPin, OUTPUT);
    setSystemLED(SystemLEDState::OFF);
    setActivityLED(ActivityLEDState::OFF);
}

void LEDController::handle() {
    unsigned long now = millis();
    // System LED flashing logic
    if (systemState == SystemLEDState::FLASHING_RED || systemState == SystemLEDState::FLASHING_GREEN) {
        if (now - lastFlashTime > 250) { // 250ms flash interval
            lastFlashTime = now;
            flashOn = !flashOn;
            digitalWrite(systemLEDPin, flashOn ? HIGH : LOW);
        }
    } else {
        if (systemState == SystemLEDState::RED) digitalWrite(systemLEDPin, HIGH);
        else if (systemState == SystemLEDState::GREEN) digitalWrite(systemLEDPin, HIGH);
        else digitalWrite(systemLEDPin, LOW);
    }
    // Activity LED logic
    if (activityState == ActivityLEDState::FLASHING) {
        if (now - lastFlashTime > 250) {
            lastFlashTime = now;
            flashOn = !flashOn;
            digitalWrite(activityLEDPin, flashOn ? HIGH : LOW);
        }
    } else {
        digitalWrite(activityLEDPin, activityState == ActivityLEDState::ON ? HIGH : LOW);
    }
}

void LEDController::setSystemLED(SystemLEDState state) {
    systemState = state;
    lastFlashTime = millis();
    flashOn = false;
}

void LEDController::setActivityLED(ActivityLEDState state) {
    activityState = state;
    lastFlashTime = millis();
    flashOn = false;
}
