#pragma once
#include <string>

class HardwareSerial {
public:
    void begin(unsigned long baud) {}
    void begin(unsigned long baud, uint32_t config, int8_t rxPin = -1, int8_t txPin = -1) {}
    void end() {}
    int available() { return 0; }
    int read() { return -1; }
    void flush() {}
    size_t write(uint8_t b) { return 1; }
    size_t write(const uint8_t* buf, size_t size) { return size; }
    size_t print(const char* str) { return 0; }
    size_t println(const char* str) { return 0; }
    size_t println(int num) { return 0; }
    size_t println(float num) { return 0; }
    size_t println() { return 0; }
};

extern HardwareSerial Serial;
