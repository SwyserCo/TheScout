#pragma once

enum class LEDPattern {
    OFF,
    ON,
    BLINK_SLOW,
    BLINK_FAST,
    PULSE
};

enum class SystemPattern {
    OFF,
    CONNECTING,
    CONNECTED,
    ERROR,
    FACTORY_RESET
};

enum class ActivityPattern {
    OFF,
    ARMED,
    ALARM
};
