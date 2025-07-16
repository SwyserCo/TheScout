#pragma once
#include <string>

class WiFiClient {
public:
    bool connect(const char* host, uint16_t port) { return true; }
    void stop() {}
    bool connected() { return true; }
    size_t write(const uint8_t* buf, size_t size) { return size; }
    size_t write(uint8_t b) { return 1; }
    int available() { return 0; }
    int read() { return -1; }
    void flush() {}
};
