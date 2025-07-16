#pragma once

#include <Arduino.h>

enum class BuzzerChime : uint8_t {
    NONE = 0,
    SUCCESS = 1,
    FAILURE = 2,
    TIMEOUT = 3
};

class Buzzer {
public:
    void begin();
    void handle();
    void playChime(BuzzerChime chime);

private:
    BuzzerChime currentChime = BuzzerChime::NONE;
    unsigned long chimeStartTime = 0;
    const uint8_t buzzerPin = 4; // Example GPIO
    void playSuccessChime();
    void playFailureChime();
    void playTimeoutChime();
};
