#pragma once

enum class LEDPattern {
    OFF,
    ON,
    BLINK_SLOW,
    BLINK_FAST,
    PULSE,
    FLASH_3X    // Flash 3 times then stop
};

enum class SystemPattern {
    OFF,
    STARTUP,
    CONNECTING,
    CONNECTED,
    ERROR,
    NORMAL,
    ALARM,
    FACTORY_RESET,
    FACTORY_RESET_PROGRESS,
    FACTORY_RESET_COMPLETE
};

enum class ActivityPattern {
    OFF,
    ARMED,
    ALARM,
    MOTION
};
