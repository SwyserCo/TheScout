#include "utility/TamperDetection.h"
#include <Arduino.h>

void TamperDetection::begin() {
    pinMode(Config::TAMPER_PIN, INPUT_PULLUP);
    lastState = digitalRead(Config::TAMPER_PIN);
    lastDebounceTime = 0;
}

void TamperDetection::update() {
    unsigned long currentTime = millis();
    int reading = digitalRead(Config::TAMPER_PIN);
    
    // If the switch changed, due to noise or pressing
    if (reading != lastState) {
        // Reset debouncing timer
        lastDebounceTime = currentTime;
    }
    
    // If enough time has passed since last change
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        // If the reading is different from the current state
        if (reading != lastState) {
            lastState = reading;
            
            // LOW means tampered
            if (tamperCallback) {
                tamperCallback(reading == LOW);
            }
        }
    }
    
    lastState = reading;
}

bool TamperDetection::isTampered() const {
    return lastState == LOW;
}

void TamperDetection::setTamperCallback(TamperCallback callback) {
    tamperCallback = callback;
}
