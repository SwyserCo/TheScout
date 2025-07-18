#pragma once

#include <Arduino.h>

enum class BuzzerPattern {
    NONE,
    SINGLE_BEEP,
    DOUBLE_BEEP,
    TRIPLE_BEEP,
    SUCCESS_CHIME,
    FAILURE_CHIME,
    ALARM_SIREN,
    WIFI_TIMEOUT,
    STARTUP_CHIME
};

class Buzzer {
public:
    Buzzer(uint8_t pin);
    void begin();
    void playPattern(BuzzerPattern pattern);
    void stop();
    void update();
    bool isPlaying() const;
    
private:
    uint8_t _pin;
    uint8_t _pwmChannel;
    BuzzerPattern _currentPattern;
    uint32_t _patternStart;
    uint32_t _lastBeep;
    uint8_t _beepCount;
    uint8_t _beepsRemaining;
    bool _isPlaying;
    bool _beepState;
    uint32_t _toneStart;
    uint32_t _toneDuration;
    
    void playTone(uint16_t frequency, uint32_t duration);
    void playBeep(uint16_t frequency = 1000, uint32_t duration = 100);
    void stopTone();
};
