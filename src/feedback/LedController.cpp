#include "feedback/LedController.h"
#include "config/Pins.h"

// Constructor - no hardware initialization per PRD
LedController::LedController() : ledsInitialized(false) {
    // Initialize all pixels to BLACK
    for (int i = 0; i < LED_COUNT; i++) {
        leds[i] = CRGB::Black;
    }
}

void LedController::begin() {
    if (ledsInitialized) return;
    
    #ifdef DEBUG
    Serial.println("[LED] === FastLED INITIALIZATION (PRD Compliant) ===");
    Serial.printf("[LED] System LED on pin %d, Activity LED on pin %d\n", SYSTEM_LED_PIN, ACTIVITY_LED_PIN);
    Serial.println("[LED] Using single CRGB array with dual addLeds calls");
    Serial.flush();
    #endif
    
    // **CRITICAL**: Aggressive GPIO setup to prevent white LED on boot
    pinMode(SYSTEM_LED_PIN, OUTPUT);
    pinMode(ACTIVITY_LED_PIN, OUTPUT);
    digitalWrite(SYSTEM_LED_PIN, LOW);
    digitalWrite(ACTIVITY_LED_PIN, LOW);
    delay(100); // Hold LEDs off
    
    #ifdef DEBUG
    Serial.println("[LED] GPIO pins set LOW and held for 100ms");
    Serial.flush();
    #endif

    // **PRD REQUIREMENT**: Initialize FastLED with single CRGB array and two addLeds calls
    FastLED.addLeds<WS2812B, SYSTEM_LED_PIN, GRB>(leds, 0, 1);      // System LED at index 0
    FastLED.addLeds<WS2812B, ACTIVITY_LED_PIN, GRB>(leds, 1, 1);    // Activity LED at index 1
    
    #ifdef DEBUG
    Serial.println("[LED] FastLED configured with dual addLeds calls");
    Serial.flush();
    #endif
    
    // Set initial brightness and ensure all LEDs are off
    FastLED.setBrightness(globalBrightness);
    FastLED.clear();
    FastLED.show();
    
    #ifdef DEBUG
    Serial.println("[LED] FastLED initialization complete - LEDs should be OFF");
    Serial.flush();
    #endif
    
    // Initialize pixel states
    for (int i = 0; i < LED_COUNT; i++) {
        pixelStates[i].animation = LedAnimation::SOLID;
        pixelStates[i].baseColor = CRGB::Black;
        pixelStates[i].interval = 0;
        pixelStates[i].lastUpdate = 0;
        pixelStates[i].isOn = false;
        pixelStates[i].pulsePhase = 0;
    }
    
    ledsInitialized = true;
    
    #ifdef DEBUG
    Serial.println("[LED] LED Controller ready - LEDs should be OFF");
    Serial.flush();
    #endif
}

void LedController::update() {
    if (!ledsInitialized || stealthMode) {
        return;
    }
    
    // Update animations for both pixels
    updatePixelAnimation(PIXEL_SYSTEM);
    updatePixelAnimation(PIXEL_ACTIVITY);
}

void LedController::setBrightness(uint8_t brightness) {
    globalBrightness = brightness;
    
    if (ledsInitialized) {
        FastLED.setBrightness(brightness);
        
        // Reapply current colors with new brightness
        for (int i = 0; i < LED_COUNT; i++) {
            if (pixelStates[i].isOn || pixelStates[i].animation != LedAnimation::SOLID) {
                updatePixelAnimation(i);
            }
        }
        FastLED.show();
    }
    
    #ifdef DEBUG
    Serial.printf("[LED] Brightness set to %d\n", brightness);
    Serial.flush();
    #endif
}

void LedController::setStealthMode(bool enabled) {
    stealthMode = enabled;
    
    #ifdef DEBUG
    Serial.printf("[LED] Stealth mode %s\n", enabled ? "ENABLED" : "DISABLED");
    Serial.flush();
    #endif
    
    if (enabled) {
        turnOff();
    }
}

void LedController::startAnimation(uint8_t pixelIndex, CRGB color, LedAnimation animation, uint16_t interval) {
    if (!ledsInitialized || stealthMode || pixelIndex >= LED_COUNT) {
        #ifdef DEBUG
        if (!ledsInitialized) Serial.println("[LED] Error: LEDs not initialized");
        if (stealthMode) Serial.println("[LED] Stealth mode active, ignoring animation");
        if (pixelIndex >= LED_COUNT) Serial.printf("[LED] Error: Invalid pixel index %d\n", pixelIndex);
        Serial.flush();
        #endif
        return;
    }
    
    #ifdef DEBUG
    const char* animationName = 
        (animation == LedAnimation::SOLID) ? "SOLID" :
        (animation == LedAnimation::BLINK) ? "BLINK" : "PULSE";
    
    Serial.printf("[LED] Pixel %d: %s animation, CRGB(%d,%d,%d), interval=%dms\n", 
                  pixelIndex, animationName, color.r, color.g, color.b, interval);
    Serial.flush();
    #endif
    
    PixelState& state = pixelStates[pixelIndex];
    state.animation = animation;
    state.baseColor = color;
    state.interval = interval;
    state.lastUpdate = 0; // Force immediate update
    state.isOn = true;
    state.pulsePhase = 0;
    
    // Apply initial color immediately
    switch (animation) {
        case LedAnimation::SOLID:
            applyPixelColor(pixelIndex, color);
            break;
        case LedAnimation::BLINK:
            applyPixelColor(pixelIndex, color); // Start with color on
            break;
        case LedAnimation::PULSE:
            applyPixelColor(pixelIndex, color); // Start at full brightness
            break;
    }
}

void LedController::turnOff(uint8_t pixelIndex) {
    if (!ledsInitialized || pixelIndex >= LED_COUNT) {
        return;
    }
    
    #ifdef DEBUG
    Serial.printf("[LED] Turning off pixel %d\n", pixelIndex);
    Serial.flush();
    #endif
    
    // Clear pixel state
    pixelStates[pixelIndex].animation = LedAnimation::SOLID;
    pixelStates[pixelIndex].baseColor = CRGB::Black;
    pixelStates[pixelIndex].isOn = false;
    pixelStates[pixelIndex].lastUpdate = 0;
    pixelStates[pixelIndex].pulsePhase = 0;
    
    // Turn off hardware
    applyPixelColor(pixelIndex, CRGB::Black);
}

void LedController::turnOff() {
    if (!ledsInitialized) {
        return;
    }
    
    #ifdef DEBUG
    Serial.println("[LED] Turning off all LEDs");
    Serial.flush();
    #endif
    
    // Clear all pixel states
    for (int i = 0; i < LED_COUNT; i++) {
        pixelStates[i].animation = LedAnimation::SOLID;
        pixelStates[i].baseColor = CRGB::Black;
        pixelStates[i].isOn = false;
        pixelStates[i].lastUpdate = 0;
        pixelStates[i].pulsePhase = 0;
    }
    
    // Turn off hardware
    FastLED.clear();
    FastLED.show();
}

void LedController::updatePixelAnimation(uint8_t pixelIndex) {
    if (pixelIndex >= LED_COUNT) {
        return;
    }
    
    PixelState& state = pixelStates[pixelIndex];
    unsigned long currentTime = millis();
    
    switch (state.animation) {
        case LedAnimation::SOLID:
            // Static color - only update once
            if (state.lastUpdate == 0) {
                applyPixelColor(pixelIndex, state.baseColor);
                state.lastUpdate = currentTime;
            }
            break;
            
        case LedAnimation::BLINK:
            if (currentTime - state.lastUpdate >= state.interval) {
                state.isOn = !state.isOn;
                state.lastUpdate = currentTime;
                
                if (state.isOn) {
                    applyPixelColor(pixelIndex, state.baseColor);
                } else {
                    applyPixelColor(pixelIndex, CRGB::Black);
                }
            }
            break;
            
        case LedAnimation::PULSE:
            if (currentTime - state.lastUpdate >= FADE_STEP_MS) {
                // Sine wave breathing effect
                uint8_t brightness = sineWave(state.pulsePhase);
                CRGB scaledColor = scaleColor(state.baseColor, brightness);
                
                applyPixelColor(pixelIndex, scaledColor);
                
                // Advance phase based on interval
                uint8_t phaseIncrement = (255 * FADE_STEP_MS) / state.interval;
                state.pulsePhase = (state.pulsePhase + phaseIncrement) % 255;
                
                state.lastUpdate = currentTime;
            }
            break;
    }
}

void LedController::applyPixelColor(uint8_t pixelIndex, CRGB color) {
    if (!ledsInitialized || pixelIndex >= LED_COUNT) {
        return;
    }
    
    if (stealthMode) {
        color = CRGB::Black;  // Black in stealth mode
    }
    
    // Apply to LED array
    leds[pixelIndex] = color;
    FastLED.show();
}

CRGB LedController::scaleColor(CRGB color, uint8_t brightness) {
    return CRGB(
        (color.r * brightness) / 255,
        (color.g * brightness) / 255,
        (color.b * brightness) / 255
    );
}

uint8_t LedController::sineWave(uint8_t phase) {
    // Proper sine wave approximation for smooth breathing
    // Creates a natural 0→peak→0→0→peak breathing pattern
    
    // Use a lookup table approach for smoother transitions
    // This creates a breathing effect with a longer pause at the bottom
    if (phase < 85) {
        // Rising phase: 0 to peak (0-255)
        // Use quadratic easing for smooth acceleration
        uint16_t scaled = (phase * 3);  // 0-255 range
        return scaled;
    } else if (phase < 170) {
        // Falling phase: peak to 0 (255-0)  
        uint8_t fallPhase = phase - 85;
        uint16_t scaled = 255 - (fallPhase * 3);
        return scaled;
    } else {
        // Bottom pause: stay at minimum brightness
        // This creates the natural "pause" in breathing
        return 0;
    }
}
