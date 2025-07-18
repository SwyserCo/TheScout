#include "utility/LEDController.h"
#include "Config.h"

LEDController::LEDController(uint8_t pin) 
    : _pin(pin), _state(LEDState::OFF), _ledOn(false), 
      _lastUpdate(0), _blinkInterval(Config::LED_BLINK_INTERVAL),
      _flashCount(0), _flashRemaining(0), _flashDuration(200),
      _flashStart(0), _flashActive(false) {
}

void LEDController::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void LEDController::setState(LEDState state) {
    _state = state;
    _lastUpdate = millis();
    
    if (state == LEDState::OFF) {
        digitalWrite(_pin, LOW);
        _ledOn = false;
        _flashActive = false;
    } else if (state == LEDState::ON) {
        digitalWrite(_pin, HIGH);
        _ledOn = true;
        _flashActive = false;
    }
}

void LEDController::update() {
    if (_flashActive) {
        uint32_t elapsed = millis() - _flashStart;
        
        if (elapsed >= _flashDuration) {
            if (_flashRemaining > 0) {
                _flashRemaining--;
                _ledOn = !_ledOn;
                digitalWrite(_pin, _ledOn ? HIGH : LOW);
                _flashStart = millis();
            } else {
                _flashActive = false;
                setState(LEDState::OFF);
            }
        }
        return;
    }
    
    uint32_t currentTime = millis();
    
    switch (_state) {
        case LEDState::BLINK:
            if (currentTime - _lastUpdate >= _blinkInterval) {
                _ledOn = !_ledOn;
                digitalWrite(_pin, _ledOn ? HIGH : LOW);
                _lastUpdate = currentTime;
            }
            break;
            
        case LEDState::FAST_BLINK:
            if (currentTime - _lastUpdate >= Config::LED_FAST_BLINK_INTERVAL) {
                _ledOn = !_ledOn;
                digitalWrite(_pin, _ledOn ? HIGH : LOW);
                _lastUpdate = currentTime;
            }
            break;
            
        case LEDState::PULSE:
            // Simple pulse implementation
            if (currentTime - _lastUpdate >= 50) {
                static uint8_t brightness = 0;
                static bool increasing = true;
                
                if (increasing) {
                    brightness += 5;
                    if (brightness >= 255) {
                        brightness = 255;
                        increasing = false;
                    }
                } else {
                    brightness -= 5;
                    if (brightness <= 0) {
                        brightness = 0;
                        increasing = true;
                    }
                }
                
                // For digital pins, approximate with PWM-like behavior
                digitalWrite(_pin, brightness > 127 ? HIGH : LOW);
                _lastUpdate = currentTime;
            }
            break;
            
        default:
            break;
    }
}

void LEDController::flashPattern(uint8_t count, uint32_t duration) {
    _flashCount = count;
    _flashRemaining = count * 2; // On and off for each flash
    _flashDuration = duration;
    _flashStart = millis();
    _flashActive = true;
    _ledOn = true;
    digitalWrite(_pin, HIGH);
}

void LEDController::setBlinkInterval(uint32_t interval) {
    _blinkInterval = interval;
}
