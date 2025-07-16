#pragma once
#include <string>

class HardwareSerial {
public:
    HardwareSerial() {}
    HardwareSerial(int port) {}
    void begin(unsigned long baud) {}
    void end() {}
    int available() { return 0; }
    int read() { return -1; }
    void flush() {}
    size_t write(uint8_t b) { return 1; }
    size_t write(const uint8_t* buf, size_t size) { return size; }
};
