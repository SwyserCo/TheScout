#include <Arduino.h>
#include "feedback/BuzzerController.h"
#include "config/Settings.h"
#include "config/Pins.h"

BuzzerController::BuzzerController() 
    : currentVolume(DEFAULT_VOLUME), stealthMode(false), lastUpdate(0) {
}

bool BuzzerController::begin() {
    Serial.println("[BUZZER] BuzzerController::begin() called");
    return true;
}

void BuzzerController::update() {
    // Serial.println("[BUZZER] BuzzerController::update() called");
}

void BuzzerController::playSuccess() {
    Serial.println("[BUZZER] BuzzerController::playSuccess() called");
}

void BuzzerController::playFailure() {
    Serial.println("[BUZZER] BuzzerController::playFailure() called");
}

void BuzzerController::playConfirm() {
    Serial.println("[BUZZER] BuzzerController::playConfirm() called");
}

void BuzzerController::playInteraction() {
    Serial.println("[BUZZER] BuzzerController::playInteraction() called");
}

void BuzzerController::setVolume(uint8_t volume) {
    currentVolume = volume;
    Serial.printf("[BUZZER] BuzzerController::setVolume(%d) called\n", volume);
}

void BuzzerController::setStealthMode(bool enabled) {
    stealthMode = enabled;
    Serial.printf("[BUZZER] BuzzerController::setStealthMode(%s) called\n", enabled ? "true" : "false");
}
