#ifdef NATIVE_TEST

#include "Arduino.h"
#include <map>

uint32_t _millis = 0;

uint32_t millis() {
    return _millis;
}

static std::map<uint8_t, uint8_t> pinStates;
static std::map<uint8_t, uint8_t> pinModes;
static std::map<uint8_t, int> analogValues;

void digitalWrite(uint8_t pin, uint8_t val) {
    pinStates[pin] = val;
}

int analogRead(uint8_t pin) {
    return analogValues[pin];
}

void analogWrite(uint8_t pin, int val) {
    analogValues[pin] = val;
}

void pinMode(uint8_t pin, uint8_t mode) {
    pinModes[pin] = mode;
}

void delay(uint32_t ms) {
    _millis += ms;
}

#endif // NATIVE_TEST
