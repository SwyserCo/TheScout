#include "utility/Buzzer.h"
#include "Config.h"

Buzzer::Buzzer(uint8_t pin) 
    : _pin(pin), _pwmChannel(0), _currentPattern(BuzzerPattern::NONE), 
      _patternStart(0), _lastBeep(0), _beepCount(0), 
      _beepsRemaining(0), _isPlaying(false), _beepState(false),
      _toneStart(0), _toneDuration(0) {
}

void Buzzer::begin() {
    // Configure PWM channel for buzzer
    ledcSetup(_pwmChannel, 1000, 8);  // 1000 Hz, 8-bit resolution
    ledcAttachPin(_pin, _pwmChannel);
    ledcWrite(_pwmChannel, 0);  // Start with buzzer off
}

void Buzzer::playPattern(BuzzerPattern pattern) {
    _currentPattern = pattern;
    _patternStart = millis();
    _lastBeep = 0;
    _beepCount = 0;
    _beepsRemaining = 0;
    _isPlaying = true;
    _beepState = false;
    
    switch (pattern) {
        case BuzzerPattern::SINGLE_BEEP:
            _beepsRemaining = 1;
            break;
        case BuzzerPattern::DOUBLE_BEEP:
            _beepsRemaining = 2;
            break;
        case BuzzerPattern::TRIPLE_BEEP:
            _beepsRemaining = 3;
            break;
        case BuzzerPattern::SUCCESS_CHIME:
            _beepsRemaining = 2;
            break;
        case BuzzerPattern::FAILURE_CHIME:
            _beepsRemaining = 3;
            break;
        case BuzzerPattern::WIFI_TIMEOUT:
            _beepsRemaining = 5;
            break;
        case BuzzerPattern::STARTUP_CHIME:
            _beepsRemaining = 3;
            break;
        case BuzzerPattern::ALARM_SIREN:
            _beepsRemaining = 255; // Continuous
            break;
        default:
            _isPlaying = false;
            break;
    }
}

void Buzzer::stop() {
    _isPlaying = false;
    _currentPattern = BuzzerPattern::NONE;
    stopTone();
}

void Buzzer::update() {
    if (!_isPlaying) return;
    
    uint32_t currentTime = millis();
    
    // Check if current tone should be stopped
    if (_toneStart > 0 && currentTime - _toneStart >= _toneDuration) {
        stopTone();
        _toneStart = 0;
    }
    
    switch (_currentPattern) {
        case BuzzerPattern::SINGLE_BEEP:
        case BuzzerPattern::DOUBLE_BEEP:
        case BuzzerPattern::TRIPLE_BEEP:
            if (_beepsRemaining > 0) {
                if (_lastBeep == 0 || currentTime - _lastBeep >= 200) {
                    playBeep();
                    _lastBeep = currentTime;
                    _beepsRemaining--;
                }
            } else {
                _isPlaying = false;
            }
            break;
            
        case BuzzerPattern::SUCCESS_CHIME:
            if (_beepsRemaining > 0) {
                if (_lastBeep == 0 || currentTime - _lastBeep >= 150) {
                    playBeep(1500, 100); // Higher pitch for success
                    _lastBeep = currentTime;
                    _beepsRemaining--;
                }
            } else {
                _isPlaying = false;
            }
            break;
            
        case BuzzerPattern::FAILURE_CHIME:
            if (_beepsRemaining > 0) {
                if (_lastBeep == 0 || currentTime - _lastBeep >= 200) {
                    playBeep(500, 150); // Lower pitch for failure
                    _lastBeep = currentTime;
                    _beepsRemaining--;
                }
            } else {
                _isPlaying = false;
            }
            break;
            
        case BuzzerPattern::WIFI_TIMEOUT:
            if (_beepsRemaining > 0) {
                if (_lastBeep == 0 || currentTime - _lastBeep >= 100) {
                    playBeep(800, 50); // Quick beeps for timeout
                    _lastBeep = currentTime;
                    _beepsRemaining--;
                }
            } else {
                _isPlaying = false;
            }
            break;
            
        case BuzzerPattern::STARTUP_CHIME:
            if (_beepsRemaining > 0) {
                if (_lastBeep == 0 || currentTime - _lastBeep >= 200) {
                    // Ascending tone sequence for startup
                    uint16_t frequency = 800 + (_beepCount * 300); // 800, 1100, 1400 Hz
                    playBeep(frequency, 150);
                    _lastBeep = currentTime;
                    _beepsRemaining--;
                    _beepCount++;
                }
            } else {
                _isPlaying = false;
            }
            break;
            
        case BuzzerPattern::ALARM_SIREN:
            if (_lastBeep == 0 || currentTime - _lastBeep >= 500) {
                // Alternating high and low tones
                uint16_t frequency = (_beepCount % 2 == 0) ? 1000 : 1500;
                playBeep(frequency, 400);
                _lastBeep = currentTime;
                _beepCount++;
            }
            break;
            
        default:
            _isPlaying = false;
            break;
    }
}

bool Buzzer::isPlaying() const {
    return _isPlaying;
}

void Buzzer::playTone(uint16_t frequency, uint32_t duration) {
    ledcWriteTone(_pwmChannel, frequency);
    _toneStart = millis();
    _toneDuration = duration;
}

void Buzzer::playBeep(uint16_t frequency, uint32_t duration) {
    ledcWriteTone(_pwmChannel, frequency);
    _toneStart = millis();
    _toneDuration = duration;
}

void Buzzer::stopTone() {
    ledcWrite(_pwmChannel, 0);
}
