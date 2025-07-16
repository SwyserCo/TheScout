#pragma once
#include "../mocks/Config.h"
#include <Arduino.h>

class Relay {
public:
    void begin() {
        pinMode(Config::RELAY_PIN, OUTPUT);
        digitalWrite(Config::RELAY_PIN, LOW);
    }

    void on() {
        digitalWrite(Config::RELAY_PIN, HIGH);
    }

    void off() {
        digitalWrite(Config::RELAY_PIN, LOW);
    }

    void toggle() {
        if (isOn()) {
            off();
        } else {
            on();
        }
    }

    bool isOn() const {
        return digitalRead(Config::RELAY_PIN) == HIGH;
    }

    void pulse(uint32_t duration) {
        on();
        pulseEndTime = millis() + duration;
    }

    void update() {
        if (pulseEndTime > 0 && millis() >= pulseEndTime) {
            off();
            pulseEndTime = 0;
        }
    }

private:
    uint32_t pulseEndTime = 0;
};
