#pragma once

#ifdef NATIVE_TEST

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <cstring>

// Basic Arduino types and functions
// Arduino String class
class String : public std::string {
public:
    String() : std::string() {}
    String(const char* str) : std::string(str) {}
    String(const std::string& str) : std::string(str) {}
    String(String&& other) : std::string(std::move(other)) {}
    String& operator=(const String& other) = default;
    String& operator=(String&& other) = default;
    String& operator=(const char* str) { std::string::operator=(str); return *this; }
};

extern uint32_t _millis;
uint32_t millis();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

// GPIO
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);
void pinMode(uint8_t pin, uint8_t mode);

// Constants
#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT 0
#define INPUT_PULLUP 2
#define LED_BUILTIN 2

// Include ESP32 ADC defines from driver/adc.h

// ESP32 I2C defines
#define I2C_FREQ_HZ 100000

// UART
#define SERIAL_8N1 0x800001c

// ESP32 ADC functions
void adc1_config_width(int width);
void adc1_config_channel_atten(int channel, int atten);
int adc1_get_raw(int channel);

#endif // NATIVE_TEST
