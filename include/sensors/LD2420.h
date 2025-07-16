#pragma once
#include <Arduino.h>

class LD2420 {
public:
    void begin();
    void read();
    bool isPresenceDetected() const;
private:
    bool presence = false;
};
