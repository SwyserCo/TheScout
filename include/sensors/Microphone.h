#pragma once
#include <Arduino.h>

class Microphone {
public:
    void begin();
    void read();
    float getSoundLevel() const;
private:
    float soundLevel = 0.0f;
};
