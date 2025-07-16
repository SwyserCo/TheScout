#pragma once
#include "../Config.h"
#include "LEDPatterns.h"
#include <Arduino.h>

class LEDController {
public:
    void begin();
    void update();
    
    // LED control
    void setSystemPattern(SystemPattern pattern);
    void setActivityPattern(ActivityPattern pattern);

private:
    void updateSystemLED(unsigned long currentTime);
    void updateActivityLED(unsigned long currentTime);

    LEDPattern systemPattern = LEDPattern::OFF;
    LEDPattern activityPattern = LEDPattern::OFF;
    unsigned long systemLastUpdate = 0;
    unsigned long activityLastUpdate = 0;
    bool systemState = false;
    bool activityState = false;
};
