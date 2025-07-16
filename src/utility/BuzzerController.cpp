#include "utility/BuzzerController.h"

void BuzzerController::begin() {
    ledcSetup(0, 2000, 8); // Channel 0, 2kHz frequency, 8-bit resolution
    ledcAttachPin(Config::BUZZER_PIN, 0);
    ledcWrite(0, 0);
}

void BuzzerController::update() {
    unsigned long currentTime = millis();
    
    if (currentPattern == BuzzerPattern::OFF) {
        return;
    }
    
    if (currentTime - lastUpdate >= patternDelay) {
        switch (currentPattern) {
            case BuzzerPattern::SINGLE_BEEP:
                if (patternStep == 0) {
                    beep(100);
                    patternStep++;
                } else {
                    stop();
                }
                break;
                
            case BuzzerPattern::DOUBLE_BEEP:
                if (patternStep < 2) {
                    beep(100);
                    patternStep++;
                    patternDelay = 200;
                } else {
                    stop();
                }
                break;
                
            case BuzzerPattern::TRIPLE_BEEP:
                if (patternStep < 3) {
                    beep(100);
                    patternStep++;
                    patternDelay = 200;
                } else {
                    stop();
                }
                break;
                
            case BuzzerPattern::CONTINUOUS:
                beep(1000);
                break;
                
            case BuzzerPattern::ALARM:
                if (currentTime - lastUpdate >= 500) {
                    if (alarmHigh) {
                        ledcWriteTone(0, 2000);
                    } else {
                        ledcWriteTone(0, 1500);
                    }
                    alarmHigh = !alarmHigh;
                    lastUpdate = currentTime;
                }
                break;
            
            default:
                stop();
                break;
        }
    }
}

void BuzzerController::playPattern(BuzzerPattern pattern) {
    currentPattern = pattern;
    patternStep = 0;
    patternDelay = 0;
    lastUpdate = millis();
    alarmHigh = false;
    isPlaying = true;
}

void BuzzerController::playChime(BuzzerChime chime) {
    currentChime = chime;
    switch (chime) {
        case BuzzerChime::SUCCESS:
            currentPattern = BuzzerPattern::DOUBLE_BEEP;
            break;
        case BuzzerChime::WARNING:
            currentPattern = BuzzerPattern::TRIPLE_BEEP;
            break;
        case BuzzerChime::ERROR:
            currentPattern = BuzzerPattern::ALARM;
            break;
    }
    playPattern(currentPattern);
}

void BuzzerController::stop() {
    currentPattern = BuzzerPattern::OFF;
    ledcWrite(0, 0);
    isPlaying = false;
}

void BuzzerController::beep(unsigned int duration) {
    ledcWriteTone(0, 2000);
    patternDelay = duration;
    lastUpdate = millis();
}
