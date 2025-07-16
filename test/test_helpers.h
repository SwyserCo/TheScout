#pragma once
#include <unity.h>
#include <Wire.h>
#include <deque>

// Testing helper functions
void advance_time(uint32_t ms) {
    extern uint32_t _millis;
    _millis += ms;
}

// Mock I2C device responses
class I2CMock {
public:
    static void clearData() {
        writeData.clear();
        readData.clear();
    }
    
    static void queueRead(uint8_t data) {
        readData.push_back(data);
    }
    
    static uint8_t getLastWrite() {
        if (writeData.empty()) return 0;
        uint8_t data = writeData.front();
        writeData.pop_front();
        return data;
    }
    
    static void write(uint8_t data) {
        writeData.push_back(data);
    }
    
    static uint8_t read() {
        if (readData.empty()) return 0;
        uint8_t data = readData.front();
        readData.pop_front();
        return data;
    }
    
private:
    static std::deque<uint8_t> writeData;
    static std::deque<uint8_t> readData;
};

std::deque<uint8_t> I2CMock::writeData;
std::deque<uint8_t> I2CMock::readData;

// The TwoWire class is defined in Wire.h
