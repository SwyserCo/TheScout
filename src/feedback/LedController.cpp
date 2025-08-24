#include "feedback/LedController.h"
#include "config/Pins.h"

LedController::LedController() {
    // Initialize pointers to nullptr - no hardware initialization yet
    systemLed = nullptr;
    activityLed = nullptr;
}

LedController::~LedController() {
    // Clean up allocated NeoPixel objects
    delete systemLed;
    delete activityLed;
}

void LedController::begin() {
    #ifdef DEBUG
    Serial.println("[LED] === HYBRID NeoPixel INITIALIZATION ===");
    Serial.printf("[LED] System LED on pin %d, Activity LED on pin %d\n", SYSTEM_LED_PIN, ACTIVITY_LED_PIN);
    Serial.println("[LED] Using hybrid approach to prevent white LED at boot");
    Serial.flush();
    #endif
    
    // STEP 1: Force GPIO control first to ensure LEDs are OFF
    pinMode(SYSTEM_LED_PIN, OUTPUT);
    pinMode(ACTIVITY_LED_PIN, OUTPUT);
    digitalWrite(SYSTEM_LED_PIN, LOW);
    digitalWrite(ACTIVITY_LED_PIN, LOW);
    delay(100); // Give time for LEDs to turn off
    
    #ifdef DEBUG
    Serial.println("[LED] GPIO pins set LOW and held for 100ms");
    Serial.flush();
    #endif
    
    // STEP 2: Create NeoPixel instances (this is where construction happens)
    // Use NEO_GRB color order (most common for WS2812B LEDs)
    systemLed = new Adafruit_NeoPixel(1, SYSTEM_LED_PIN, NEO_GRB + NEO_KHZ800);
    activityLed = new Adafruit_NeoPixel(1, ACTIVITY_LED_PIN, NEO_GRB + NEO_KHZ800);
    
    #ifdef DEBUG
    Serial.println("[LED] NeoPixel objects created");
    Serial.flush();
    #endif
    
    // STEP 3: Initialize but immediately force OFF
    systemLed->begin();
    activityLed->begin();
    
    // STEP 4: Aggressive shutdown sequence
    systemLed->setBrightness(255);
    activityLed->setBrightness(255);
    
    // Multiple OFF commands using different methods
    systemLed->clear();
    activityLed->clear();
    systemLed->show();
    activityLed->show();
    delay(10);
    
    systemLed->setPixelColor(0, 0, 0, 0);
    activityLed->setPixelColor(0, 0, 0, 0);
    systemLed->show();
    activityLed->show();
    delay(10);
    
    systemLed->setPixelColor(0, systemLed->Color(0, 0, 0));
    activityLed->setPixelColor(0, activityLed->Color(0, 0, 0));
    systemLed->show();
    activityLed->show();
    
    #ifdef DEBUG
    Serial.println("[LED] NeoPixel initialization complete with aggressive OFF sequence");
    Serial.flush();
    #endif
    
    // Initialize pixel states
    for (int i = 0; i < NUM_PIXELS; i++) {
        pixelStates[i].animation = LedAnimation::SOLID;
        pixelStates[i].baseColor = {0, 0, 0};
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
        systemLed->setBrightness(255);  // Keep hardware at full, do software scaling
        activityLed->setBrightness(255);
        
        // Reapply current colors with new brightness
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (pixelStates[i].isOn || pixelStates[i].animation != LedAnimation::SOLID) {
                updatePixelAnimation(i);
            }
        }
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

void LedController::startAnimation(uint8_t pixelIndex, HearthGuardColors::RGBColor color, LedAnimation animation, uint16_t interval) {
    if (!ledsInitialized || stealthMode || pixelIndex >= NUM_PIXELS) {
        #ifdef DEBUG
        if (!ledsInitialized) Serial.println("[LED] Error: LEDs not initialized");
        if (stealthMode) Serial.println("[LED] Stealth mode active, ignoring animation");
        if (pixelIndex >= NUM_PIXELS) Serial.printf("[LED] Error: Invalid pixel index %d\n", pixelIndex);
        Serial.flush();
        #endif
        return;
    }
    
    #ifdef DEBUG
    const char* animationName = 
        (animation == LedAnimation::SOLID) ? "SOLID" :
        (animation == LedAnimation::BLINK) ? "BLINK" : "PULSE";
    
    Serial.printf("[LED] Pixel %d: %s animation, RGB(%d,%d,%d), interval=%dms\n", 
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

void LedController::turnOff() {
    if (!ledsInitialized) {
        return;
    }
    
    #ifdef DEBUG
    Serial.println("[LED] Turning off all LEDs");
    Serial.flush();
    #endif
    
    // Clear all pixel states
    for (int i = 0; i < NUM_PIXELS; i++) {
        pixelStates[i].animation = LedAnimation::SOLID;
        pixelStates[i].baseColor = {0, 0, 0};  // Black
        pixelStates[i].isOn = false;
        pixelStates[i].lastUpdate = 0;
        pixelStates[i].pulsePhase = 0;
    }
    
    // Turn off hardware
    HearthGuardColors::RGBColor black = {0, 0, 0};
    applyPixelColor(PIXEL_SYSTEM, black);
    applyPixelColor(PIXEL_ACTIVITY, black);
}

void LedController::updatePixelAnimation(uint8_t pixelIndex) {
    if (pixelIndex >= NUM_PIXELS) {
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
                    HearthGuardColors::RGBColor black = {0, 0, 0};
                    applyPixelColor(pixelIndex, black);
                }
            }
            break;
            
        case LedAnimation::PULSE:
            if (currentTime - state.lastUpdate >= FADE_STEP_MS) {
                // Sine wave breathing effect
                uint8_t brightness = sineWave(state.pulsePhase);
                HearthGuardColors::RGBColor scaledColor = scaleColor(state.baseColor, brightness);
                
                applyPixelColor(pixelIndex, scaledColor);
                
                // Advance phase based on interval
                uint8_t phaseIncrement = (255 * FADE_STEP_MS) / state.interval;
                state.pulsePhase = (state.pulsePhase + phaseIncrement) % 255;
                
                state.lastUpdate = currentTime;
            }
            break;
    }
}

void LedController::applyPixelColor(uint8_t pixelIndex, HearthGuardColors::RGBColor color) {
    if (stealthMode) {
        color = {0, 0, 0};  // Black in stealth mode
    }
    
    // Scale color by global brightness
    color = scaleColor(color, globalBrightness);
    
    // Apply to appropriate pixel
    if (pixelIndex == PIXEL_SYSTEM && ledsInitialized) {
        uint32_t pixelColor = rgbToColor(color, PIXEL_SYSTEM);
        systemLed->setPixelColor(0, pixelColor);
        systemLed->show();
    } else if (pixelIndex == PIXEL_ACTIVITY && ledsInitialized) {
        uint32_t pixelColor = rgbToColor(color, PIXEL_ACTIVITY);
        activityLed->setPixelColor(0, pixelColor);
        activityLed->show();
    }
}

HearthGuardColors::RGBColor LedController::scaleColor(HearthGuardColors::RGBColor color, uint8_t brightness) {
    return {
        (uint8_t)((color.r * brightness) / 255),
        (uint8_t)((color.g * brightness) / 255),
        (uint8_t)((color.b * brightness) / 255)
    };
}

uint32_t LedController::rgbToColor(HearthGuardColors::RGBColor color, uint8_t pixelIndex) {
    if (pixelIndex == PIXEL_SYSTEM) {
        return systemLed->Color(color.r, color.g, color.b);
    } else if (pixelIndex == PIXEL_ACTIVITY) {
        return activityLed->Color(color.r, color.g, color.b);
    }
    return 0; // Black fallback
}

uint8_t LedController::sineWave(uint8_t phase) {
    // Simple sine wave approximation using a quarter-wave lookup
    // This provides a smooth breathing effect
    if (phase < 64) {        // First quarter: 0 to peak
        return phase * 4;    // 0-255
    } else if (phase < 128) { // Second quarter: peak to 0
        return 255 - ((phase - 64) * 4);
    } else if (phase < 192) { // Third quarter: 0 to minimum (but we stay at 0)
        return 0;
    } else {                 // Fourth quarter: rise back to peak
        return ((phase - 192) * 4);
    }
}
