#pragma once

#ifdef NATIVE_TEST

#include <cstdint>
#include <string>

using String = std::string;

extern uint32_t _millis;
uint32_t millis();

void digitalWrite(uint8_t pin, uint8_t val);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);
void pinMode(uint8_t pin, uint8_t mode);
void delay(uint32_t ms);

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT 0
#define LED_BUILTIN 2

#endif // NATIVE_TEST
