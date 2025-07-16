#pragma once
#include <Arduino.h>
#include "Config.h"

class LD2420 {
public:
    void begin();
    void read();
    bool isPresenceDetected() const;
private:
    bool presence = false;
    const uint8_t rxPin = PIN_LD2420_RX;
    const uint8_t txPin = PIN_LD2420_TX;
    const uint8_t intPin = PIN_LD2420_INT;
};
