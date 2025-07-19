#include "modules/Feedback.h"
#include "config.h"

// LEDController Implementation for WS2812B
LEDController::LEDController(uint8_t ledPin, CRGB* ledArray) : 
    pin(ledPin), 
    leds(ledArray),
    lastToggle(0), 
    interval(0), 
    state(false), 
    enabled(true),
    currentColor(CRGB::White),
    offColor(CRGB::Black) {
}

void LEDController::begin() {
    // Clear the LED initially
    leds[0] = CRGB::Black;
}

void LEDController::update() {
    if (!enabled || interval == 0) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastToggle >= interval) {
        state = !state;
        leds[0] = state ? currentColor : offColor;
        FastLED.show();
        lastToggle = currentTime;
    }
}

void LEDController::setBlink(unsigned long blinkInterval, CRGB color) {
    currentColor = color;
    interval = blinkInterval;
    lastToggle = millis();
}

void LEDController::setColor(CRGB color) {
    currentColor = color;
    if (enabled) {
        leds[0] = color;
        FastLED.show();
    }
}

void LEDController::setOn(CRGB color) {
    interval = 0;
    state = true;
    currentColor = color;
    if (enabled) {
        leds[0] = color;
        FastLED.show();
    }
}

void LEDController::setOff() {
    interval = 0;
    state = false;
    leds[0] = CRGB::Black;
    FastLED.show();
}

void LEDController::enable() {
    enabled = true;
}

void LEDController::disable() {
    enabled = false;
    leds[0] = CRGB::Black;
    FastLED.show();
}

// BuzzerController Implementation
BuzzerController::BuzzerController(uint8_t buzzerPin) : pin(buzzerPin), startTime(0), duration(0), active(false), enabled(true) {}

void BuzzerController::begin() {
    // No need to set pinMode for tone() - it handles PWM setup automatically
    // Just ensure it's off initially
    noTone(pin);
}

void BuzzerController::update() {
    if (!active || !enabled) return;
    
    unsigned long currentTime = millis();
    if (currentTime - startTime >= duration) {
        stop();
    }
}

void BuzzerController::beep(unsigned long beepDuration, uint16_t frequency) {
    if (!enabled) return;
    
    duration = beepDuration;
    startTime = millis();
    active = true;
    
    // Use tone() for proper PWM buzzer sound
    tone(pin, frequency);
}

void BuzzerController::stop() {
    active = false;
    noTone(pin);  // Stop PWM tone properly
}

void BuzzerController::enable() {
    enabled = true;
}

void BuzzerController::disable() {
    enabled = false;
    stop();  // This will call noTone() now
}

bool BuzzerController::isActive() {
    return active;
}
