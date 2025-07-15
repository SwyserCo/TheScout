#pragma once
#include <Arduino.h>

class BuzzerController {
public:
    BuzzerController();
    void begin();
    void beep(uint16_t frequency = 2000, uint16_t duration = 200);
    void stop();

private:
    static const uint8_t BUZZER_PIN = 40;
    static const uint8_t PWM_CHANNEL = 0;
};
