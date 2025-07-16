#pragma once

#include <Arduino.h>

enum class RelayState : uint8_t {
    OFF = 0,
    ON = 1
};

class Relay {
public:
    void begin();
    void handle();
    void setState(RelayState state);
    void toggle();
    RelayState getState() const;

private:
    RelayState state = RelayState::OFF;
    uint8_t relayPin = 5; // Example GPIO, update as needed
    unsigned long lastToggleTime = 0;
};
