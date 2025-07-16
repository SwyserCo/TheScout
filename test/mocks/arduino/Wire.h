#pragma once

#ifdef NATIVE_TEST

#include <cstdint>
#include <vector>
#include "Arduino.h"

class TwoWire {
public:
    void begin(int sda = -1, int scl = -1, uint32_t frequency = I2C_FREQ_HZ) {}
    void setClock(uint32_t frequency) {}
    void beginTransmission(uint8_t address);
    uint8_t endTransmission(bool sendStop = true);
    size_t write(uint8_t data);
    size_t write(const uint8_t* data, size_t length);
    uint8_t requestFrom(uint8_t address, size_t size, bool sendStop = true);
    int available();
    int read();

private:
    std::vector<uint8_t> rxBuffer;
    std::vector<uint8_t> txBuffer;
    uint8_t address = 0;
};

extern TwoWire Wire;

#endif // NATIVE_TEST
