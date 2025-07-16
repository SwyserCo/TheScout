#ifdef NATIVE_TEST

#include "Wire.h"

TwoWire Wire;

void TwoWire::begin(int sda, int scl, uint32_t frequency) {
    // No-op in mock
}

void TwoWire::beginTransmission(uint8_t addr) {
    address = addr;
    txBuffer.clear();
}

uint8_t TwoWire::endTransmission(bool sendStop) {
    // Return success
    return 0;
}

size_t TwoWire::write(uint8_t data) {
    txBuffer.push_back(data);
    return 1;
}

size_t TwoWire::write(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        txBuffer.push_back(data[i]);
    }
    return length;
}

uint8_t TwoWire::requestFrom(uint8_t addr, size_t size, bool sendStop) {
    // Mock successful read
    rxBuffer.resize(size);
    return size;
}

int TwoWire::available() {
    return rxBuffer.size();
}

int TwoWire::read() {
    if (rxBuffer.empty()) {
        return -1;
    }
    uint8_t data = rxBuffer.front();
    rxBuffer.erase(rxBuffer.begin());
    return data;
}

#endif // NATIVE_TEST
