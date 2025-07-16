#pragma once
#include <Arduino.h>
#include "Config.h"

class Microphone {
public:
    void begin();
    void read();
    float getSoundLevel() const;
private:
    float soundLevel = 0.0f;
    const uint8_t micPin = PIN_SPL_MIC;
};
