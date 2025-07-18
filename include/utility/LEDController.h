#pragma once

#include <Arduino.h>

enum class LEDState {
    OFF,
    ON,
    BLINK,
    FAST_BLINK,
    PULSE
};

class LEDController {
public:
    LEDController(uint8_t pin);
    void begin();
    void setState(LEDState state);
    void update();
    void flashPattern(uint8_t count, uint32_t duration = 200);
    void setBlinkInterval(uint32_t interval);
    
private:
    uint8_t _pin;
    LEDState _state;
    bool _ledOn;
    uint32_t _lastUpdate;
    uint32_t _blinkInterval;
    uint8_t _flashCount;
    uint8_t _flashRemaining;
    uint32_t _flashDuration;
    uint32_t _flashStart;
    bool _flashActive;
};
