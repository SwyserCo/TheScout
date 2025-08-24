#include <Arduino.h>
#include "feedback/FeedbackManager.h"
#include "config/Settings.h"

FeedbackManager::FeedbackManager() 
    : currentBrightness(DEFAULT_LED_BRIGHTNESS), stealthMode(false) {
}

bool FeedbackManager::begin() {
    Serial.println("[FEEDBACK] FeedbackManager::begin() called");
    
    bool success = true;
    
    if (!ledController.begin()) {
        Serial.println("[FEEDBACK] Failed to initialize LED Controller");
        success = false;
    }
    
    if (!buzzerController.begin()) {
        Serial.println("[FEEDBACK] Failed to initialize Buzzer Controller");
        success = false;
    }
    
    return success;
}

void FeedbackManager::update() {
    ledController.update();
    buzzerController.update();
}

void FeedbackManager::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    ledController.setBrightness(brightness);
    Serial.printf("[FEEDBACK] FeedbackManager::setBrightness(%d) called\n", brightness);
}

void FeedbackManager::setStealthMode(bool enabled) {
    stealthMode = enabled;
    buzzerController.setStealthMode(enabled);
    Serial.printf("[FEEDBACK] FeedbackManager::setStealthMode(%s) called\n", enabled ? "true" : "false");
}

void FeedbackManager::startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation, uint16_t interval) {
    ledController.startAnimation(pixelIndex, color, animation, interval);
    Serial.println("[FEEDBACK] FeedbackManager::startAnimation() called");
}

void FeedbackManager::turnOffLeds() {
    ledController.turnOffAll();
    Serial.println("[FEEDBACK] FeedbackManager::turnOffLeds() called");
}

void FeedbackManager::playSuccess() {
    buzzerController.playSuccess();
    Serial.println("[FEEDBACK] FeedbackManager::playSuccess() called");
}

void FeedbackManager::playFailure() {
    buzzerController.playFailure();
    Serial.println("[FEEDBACK] FeedbackManager::playFailure() called");
}

void FeedbackManager::playConfirm() {
    buzzerController.playConfirm();
    Serial.println("[FEEDBACK] FeedbackManager::playConfirm() called");
}

void FeedbackManager::playInteraction() {
    buzzerController.playInteraction();
    Serial.println("[FEEDBACK] FeedbackManager::playInteraction() called");
}
