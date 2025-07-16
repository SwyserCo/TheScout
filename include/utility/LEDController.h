#pragma once

#include <Arduino.h>

enum class SystemLEDState : uint8_t {
    OFF = 0,
    RED = 1,
    GREEN = 2,
    FLASHING_RED = 3,
    FLASHING_GREEN = 4
};

enum class ActivityLEDState : uint8_t {
    OFF = 0,
    ON = 1,
    FLASHING = 2
};

class LEDController {
public:
    void begin();
    void handle();
    void setSystemLED(SystemLEDState state);
    void setActivityLED(ActivityLEDState state);

private:
    SystemLEDState systemState = SystemLEDState::OFF;
    ActivityLEDState activityState = ActivityLEDState::OFF;
    unsigned long lastFlashTime = 0;
    bool flashOn = false;
    const uint8_t systemLEDPin = 2; // Example GPIO
    const uint8_t activityLEDPin = 3; // Example GPIO
};
