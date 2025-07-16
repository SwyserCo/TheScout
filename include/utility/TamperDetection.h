#pragma once
#include "../Config.h"
#include <Arduino.h>

class TamperDetection {
public:
    void begin() {
        pinMode(Config::TAMPER_PIN, INPUT_PULLUP);
        lastState = digitalRead(Config::TAMPER_PIN);
    }
    
    void update() {
        unsigned long currentTime = millis();
        if (currentTime - lastDebounceTime > debounceDelay) {
            int reading = digitalRead(Config::TAMPER_PIN);
            
            if (reading != lastState) {
                lastDebounceTime = currentTime;
                lastState = reading;
                
                if (tamperCallback) {
                    tamperCallback(reading == LOW);
                }
            }
        }
    }
    
    bool isTampered() const {
        return lastState == LOW;
    }
    
    using TamperCallback = std::function<void(bool)>;
    void setTamperCallback(TamperCallback callback) {
        tamperCallback = callback;
    }
    
private:
    int lastState = HIGH;
    unsigned long lastDebounceTime = 0;
    static constexpr unsigned long debounceDelay = 50;
    TamperCallback tamperCallback;
};
