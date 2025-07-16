#include "sensors/Relay.h"
#include <Arduino.h>

void Relay::begin() {
    pinMode(relayPin, OUTPUT);
    setState(RelayState::OFF);
}

void Relay::handle() {
    // Non-blocking logic placeholder
    // Example: debounce/timed toggle logic can go here
}

void Relay::setState(RelayState newState) {
    state = newState;
    digitalWrite(relayPin, (state == RelayState::ON) ? HIGH : LOW);
    lastToggleTime = millis();
}

void Relay::toggle() {
    setState(state == RelayState::ON ? RelayState::OFF : RelayState::ON);
}

RelayState Relay::getState() const {
    return state;
}
