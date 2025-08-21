#include "feedback/LedController.h"
#include "config/Pins.h"
#include "config/Settings.h"

LedController::LedController() 
    : systemLed(1, SYSTEM_LED_PIN, NEO_GRB + NEO_KHZ800),
      activityLed(1, ACTIVITY_LED_PIN, NEO_GRB + NEO_KHZ800),
      globalBrightness(DEFAULT_LED_BRIGHTNESS),
      stealthMode(false) {
    
    // Initialize LED states
    systemState = {PATTERN_OFF, GUARDIAN_OFF, 0, 0, 0, 0, false, 1};
    activityState = {PATTERN_OFF, GUARDIAN_OFF, 0, 0, 0, 0, false, 1};
}

void LedController::begin() {
    Serial.println("[LED] Initializing LED Controller...");
    
    systemLed.begin();
    activityLed.begin();
    
    systemLed.setBrightness(globalBrightness);
    activityLed.setBrightness(globalBrightness);
    
    systemLed.clear();
    activityLed.clear();
    systemLed.show();
    activityLed.show();
    
    Serial.printf("[LED] Controller initialized - Brightness: %d, Stealth: %s\n", 
                  globalBrightness, stealthMode ? "ON" : "OFF");
}

void LedController::update() {
    if (stealthMode) return;
    
    updatePattern(systemLed, systemState, PIXEL_SYSTEM);
    updatePattern(activityLed, activityState, PIXEL_ACTIVITY);
}

void LedController::updatePattern(Adafruit_NeoPixel& strip, LedState& state, uint8_t pixelIndex) {
    unsigned long currentTime = millis();
    
    switch (state.pattern) {
        case PATTERN_OFF:
            break;
            
        case PATTERN_SOLID:
            if (!state.isOn) {
                strip.setPixelColor(0, dimColor(state.color, globalBrightness));
                strip.show();
                state.isOn = true;
            }
            break;
            
        case PATTERN_BLINK:
            if (currentTime - state.lastUpdate >= state.interval) {
                state.isOn = !state.isOn;
                strip.setPixelColor(0, state.isOn ? dimColor(state.color, globalBrightness) : 0);
                strip.show();
                state.lastUpdate = currentTime;
            }
            break;
            
        case PATTERN_PULSE:
            if (currentTime - state.lastUpdate >= state.speed) {
                uint8_t pulseBrightness = calculatePulseBrightness(state);
                strip.setPixelColor(0, dimColor(state.color, (globalBrightness * pulseBrightness) / 255));
                strip.show();
                state.lastUpdate = currentTime;
            }
            break;
    }
}

uint32_t LedController::dimColor(uint32_t color, uint8_t brightness) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;
    
    return (r << 16) | (g << 8) | b;
}

uint8_t LedController::calculatePulseBrightness(LedState& state) {
    state.brightness += state.pulseDirection * 5;
    
    if (state.brightness <= 0) {
        state.brightness = 0;
        state.pulseDirection = 1;
    } else if (state.brightness >= 255) {
        state.brightness = 255;
        state.pulseDirection = -1;
    }
    
    return state.brightness;
}

void LedController::setBrightness(uint8_t brightness) {
    globalBrightness = brightness;
    systemLed.setBrightness(brightness);
    activityLed.setBrightness(brightness);
    Serial.printf("[LED] Brightness set to: %d\n", brightness);
}

void LedController::setStealthMode(bool enabled) {
    stealthMode = enabled;
    if (enabled) {
        turnOff();
    }
    Serial.printf("[LED] Stealth mode: %s\n", enabled ? "ENABLED" : "DISABLED");
}

uint8_t LedController::getBrightness() const {
    return globalBrightness;
}

bool LedController::getStealthMode() const {
    return stealthMode;
}

void LedController::setPixelColor(uint8_t pixelIndex, uint32_t color) {
    if (stealthMode) return;
    
    if (pixelIndex == PIXEL_SYSTEM) {
        systemState = {PATTERN_SOLID, color, 0, 0, 0, 0, false, 1};
        systemLed.setPixelColor(0, dimColor(color, globalBrightness));
        systemLed.show();
    } else if (pixelIndex == PIXEL_ACTIVITY) {
        activityState = {PATTERN_SOLID, color, 0, 0, 0, 0, false, 1};
        activityLed.setPixelColor(0, dimColor(color, globalBrightness));
        activityLed.show();
    }
}

void LedController::setSystemLed(uint32_t color) {
    setPixelColor(PIXEL_SYSTEM, color);
}

void LedController::setActivityLed(uint32_t color) {
    setPixelColor(PIXEL_ACTIVITY, color);
}

void LedController::blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval) {
    if (stealthMode) return;
    
    if (pixelIndex == PIXEL_SYSTEM) {
        systemState = {PATTERN_BLINK, color, interval, 0, millis(), 0, false, 1};
    } else if (pixelIndex == PIXEL_ACTIVITY) {
        activityState = {PATTERN_BLINK, color, interval, 0, millis(), 0, false, 1};
    }
}

void LedController::pulsePixel(uint8_t pixelIndex, uint32_t color, uint16_t speed) {
    if (stealthMode) return;
    
    if (pixelIndex == PIXEL_SYSTEM) {
        systemState = {PATTERN_PULSE, color, 0, speed, millis(), 0, false, 1};
    } else if (pixelIndex == PIXEL_ACTIVITY) {
        activityState = {PATTERN_PULSE, color, 0, speed, millis(), 0, false, 1};
    }
}

void LedController::blinkSystem(uint32_t color, uint16_t interval) {
    blinkPixel(PIXEL_SYSTEM, color, interval);
}

void LedController::blinkActivity(uint32_t color, uint16_t interval) {
    blinkPixel(PIXEL_ACTIVITY, color, interval);
}

void LedController::pulseSystem(uint32_t color, uint16_t speed) {
    pulsePixel(PIXEL_SYSTEM, color, speed);
}

void LedController::pulseActivity(uint32_t color, uint16_t speed) {
    pulsePixel(PIXEL_ACTIVITY, color, speed);
}

void LedController::turnOff() {
    turnOffSystem();
    turnOffActivity();
}

void LedController::turnOffPixel(uint8_t pixelIndex) {
    if (pixelIndex == PIXEL_SYSTEM) {
        turnOffSystem();
    } else if (pixelIndex == PIXEL_ACTIVITY) {
        turnOffActivity();
    }
}

void LedController::turnOffSystem() {
    systemState.pattern = PATTERN_OFF;
    systemLed.clear();
    systemLed.show();
}

void LedController::turnOffActivity() {
    activityState.pattern = PATTERN_OFF;
    activityLed.clear();
    activityLed.show();
}

bool LedController::isSystemOn() const {
    return systemState.pattern != PATTERN_OFF;
}

bool LedController::isActivityOn() const {
    return activityState.pattern != PATTERN_OFF;
}
