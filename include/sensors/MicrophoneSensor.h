#pragma once
#include "BaseSensor.h"
#include "../Config.h"
#include <driver/adc.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <Arduino.h>

class MicrophoneSensor : public BaseSensor {
public:
    bool begin() override;
    bool read() override;
    bool isReady() const override;
    const char* getLastError() const override;
    
    float getRMSLevel() const;
    int getPeakLevel() const;
    const std::vector<int>& getSamples() const;

private:
    std::vector<int> samples;
    float rmsLevel = 0;
    int peakLevel = 0;
    bool initialized = false;
    const char* lastError = nullptr;
};
