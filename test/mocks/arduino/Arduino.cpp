#ifdef NATIVE_TEST

#include "Arduino.h"
#include <map>

uint32_t _millis = 0;
static std::map<uint8_t, uint8_t> pinStates;
static std::map<uint8_t, uint8_t> pinModes;
static std::map<uint8_t, int> analogValues;
static int adcWidth = 12;
static std::map<int, int> adcAttenuation;

uint32_t millis() {
    return _millis;
}

void delay(uint32_t ms) {
    _millis += ms;
}

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

void adc1_config_width(int width) {
    adcWidth = width;
}

void adc1_config_channel_atten(int channel, int atten) {
    adcAttenuation[channel] = atten;
}

int adc1_get_raw(int channel) {
    return analogValues[channel];
}

#endif // NATIVE_TEST
