#pragma once
#include <cstdint>
#include <vector>

class EEPROMClass {
public:
    void begin(size_t size) {
        data.resize(size);
    }
    
    uint8_t read(int address) {
        if (address < 0 || address >= data.size()) return 0;
        return data[address];
    }
    
    void write(int address, uint8_t val) {
        if (address < 0 || address >= data.size()) return;
        data[address] = val;
    }
    
    void commit() {}
    
private:
    std::vector<uint8_t> data;
};

extern EEPROMClass EEPROM;
