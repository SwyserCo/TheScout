#include <Arduino.h>
#include "feedback/LedController.h"
#include "config/Settings.h"
#include "config/Pins.h"

LedController::LedController() 
    : currentBrightness(DEFAULT_LED_BRIGHTNESS), lastUpdate(0) {
}

bool LedController::begin() {
    Serial.println("[LED] LedController::begin() called");
    return true;
}

void LedController::update() {
    // Serial.println("[LED] LedController::update() called");
}

void LedController::setPixelColor(uint8_t pixelIndex, uint32_t color) {
    Serial.println("[LED] LedController::setPixelColor() called");
}

void LedController::startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation, uint16_t interval) {
    Serial.println("[LED] LedController::startAnimation() called");
}

void LedController::turnOffAll() {
    Serial.println("[LED] LedController::turnOffAll() called");
}

void LedController::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    Serial.printf("[LED] LedController::setBrightness(%d) called\n", brightness);
}
