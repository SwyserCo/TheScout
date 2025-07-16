#pragma once
#include "../Config.h"
#include "BuzzerPatterns.h"
#include <Arduino.h>

class BuzzerController {
public:
    void begin();
    void update();
    void playPattern(BuzzerPattern pattern);
    void playChime(BuzzerChime chime);
    void stop();

private:
    void updatePattern();
    void beep(unsigned int duration);

    unsigned long lastUpdate = 0;
    BuzzerPattern currentPattern = BuzzerPattern::OFF;
    BuzzerChime currentChime = BuzzerChime::SUCCESS;
    bool isPlaying = false;
    uint8_t patternStep = 0;
    bool alarmHigh = false;
    unsigned int patternDelay = 0;
};
