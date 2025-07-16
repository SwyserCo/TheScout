#include "utility/LEDController.h"

void LEDController::begin() {
    pinMode(Config::SYSTEM_LED, OUTPUT);
    pinMode(Config::ACTIVITY_LED, OUTPUT);
    
    // Initialize all LEDs to OFF
    digitalWrite(Config::SYSTEM_LED, LOW);
    digitalWrite(Config::ACTIVITY_LED, LOW);
}

void LEDController::update() {
    unsigned long currentTime = millis();
    updateSystemLED(currentTime);
    updateActivityLED(currentTime);
}

void LEDController::updateSystemLED(unsigned long currentTime) {
    static uint8_t flashCount = 0;
    
    switch (systemPattern) {
        case LEDPattern::OFF:
            digitalWrite(Config::SYSTEM_LED, LOW);
            break;
            
        case LEDPattern::ON:
            digitalWrite(Config::SYSTEM_LED, HIGH);
            break;
            
        case LEDPattern::BLINK_SLOW:
            if (currentTime - systemLastUpdate >= 1000) {
                systemState = !systemState;
                digitalWrite(Config::SYSTEM_LED, systemState);
                systemLastUpdate = currentTime;
            }
            break;
            
        case LEDPattern::BLINK_FAST:
            if (currentTime - systemLastUpdate >= 200) {
                systemState = !systemState;
                digitalWrite(Config::SYSTEM_LED, systemState);
                systemLastUpdate = currentTime;
            }
            break;
            
        case LEDPattern::PULSE:
            // For factory reset indication
            if (currentTime - systemLastUpdate >= 100) {
                systemState = !systemState;
                digitalWrite(Config::SYSTEM_LED, systemState);
                systemLastUpdate = currentTime;
            }
            break;
            
        case LEDPattern::FLASH_3X:
            if (currentTime - systemLastUpdate >= 200) {
                systemState = !systemState;
                if (!systemState) {  // On falling edge
                    flashCount++;
                    if (flashCount >= 6) {  // 3 complete cycles
                        systemPattern = LEDPattern::OFF;
                        flashCount = 0;
                        break;
                    }
                }
                digitalWrite(Config::SYSTEM_LED, systemState);
                systemLastUpdate = currentTime;
            }
            break;
    }
}

void LEDController::updateActivityLED(unsigned long currentTime) {
    switch (activityPattern) {
        case LEDPattern::OFF:
            digitalWrite(Config::ACTIVITY_LED, LOW);
            break;
            
        case LEDPattern::ON:
            digitalWrite(Config::ACTIVITY_LED, HIGH);
            break;
            
        case LEDPattern::BLINK_FAST:
            if (currentTime - activityLastUpdate >= 200) {
                activityState = !activityState;
                digitalWrite(Config::ACTIVITY_LED, activityState);
                activityLastUpdate = currentTime;
            }
            break;
            
        default:
            digitalWrite(Config::ACTIVITY_LED, LOW);
            break;
    }
}

void LEDController::setSystemPattern(SystemPattern pattern) {
    switch (pattern) {
        case SystemPattern::OFF:
            systemPattern = LEDPattern::OFF;
            break;
        case SystemPattern::CONNECTING:
            systemPattern = LEDPattern::BLINK_SLOW;
            break;
        case SystemPattern::CONNECTED:
            systemPattern = LEDPattern::FLASH_3X;  // Flash 3 times for successful connection
            break;
        case SystemPattern::ERROR:
            systemPattern = LEDPattern::BLINK_FAST;
            break;
        case SystemPattern::FACTORY_RESET:
            systemPattern = LEDPattern::PULSE;
            break;
    }
    systemLastUpdate = 0;
    systemState = false;
}

void LEDController::setActivityPattern(ActivityPattern pattern) {
    switch (pattern) {
        case ActivityPattern::OFF:
            activityPattern = LEDPattern::OFF;
            break;
        case ActivityPattern::ARMED:
            activityPattern = LEDPattern::ON;
            break;
        case ActivityPattern::ALARM:
            activityPattern = LEDPattern::BLINK_FAST;
            break;
    }
    activityLastUpdate = 0;
    activityState = false;
}
