#pragma once

enum class BuzzerPattern {
    OFF,
    SINGLE_BEEP,      // Simple feedback
    DOUBLE_BEEP,      // Warning or attention needed
    TRIPLE_BEEP,      // Success feedback
    CONTINUOUS,       // Continuous tone
    ALARM,           // Main alarm pattern
    ALARM_SIREN,     // Alternating high-low siren
    FACTORY_RESET,   // Special pattern for factory reset confirmation
    TAMPER_ALERT     // Special pattern for tamper detection
};

enum class BuzzerChime {
    STARTUP,         // Startup chime
    SUCCESS,         // Triple beep ascending
    ERROR,          // Double beep descending
    WIFI_CONNECTED, // Success chime for WiFi connection
    RESET_START,    // Pattern when reset button is held
    RESET_COMPLETE, // Pattern when reset is complete
    MOTION          // Quick chime for motion detection
};
