#pragma once
#include <Arduino.h>

class VEML7700 {
public:
    void begin();
    void read();
    float getAmbientLight() const;
private:
    float ambientLight = 0.0f;
};
