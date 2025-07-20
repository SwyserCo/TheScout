#include "feedback/FeedbackController.h"

void FeedbackController::begin() {
    // Initialize FastLED for WS2812B LEDs
    FastLED.addLeds<WS2812B, Pins::SYSTEM_LED, GRB>(systemLED, 1);
    FastLED.addLeds<WS2812B, Pins::ACTIVITY_LED, GRB>(activityLED, 1);
    
    // Set brightness from configuration
    FastLED.setBrightness(ledBrightness);
    systemLED[0] = CRGB::Black;
    activityLED[0] = CRGB::Black;
    FastLED.show();
    
    // Initialize PWM for buzzer using LEDC
    ledcSetup(BUZZER_CHANNEL, 1000, 8);  // Channel 0, 1kHz base frequency, 8-bit resolution
    ledcAttachPin(Pins::BUZZER, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0);  // Ensure buzzer is off
    
    Serial.println("[FeedbackController] Initialized - WS2812B LEDs and Medieval chimes ready!");
    if (stealthMode) {
        Serial.println("[FeedbackController] Stealth Mode ENABLED - LEDs and buzzer disabled");
    }
}

void FeedbackController::update() {
    unsigned long currentTime = millis();
    
    // Update LED states (check stealth mode and LED enable)
    if (!stealthMode && ledsEnabled) {
        updateLED(Pins::SYSTEM_LED, systemState, systemLED);
        updateLED(Pins::ACTIVITY_LED, activityState, activityLED);
        
        // Check for pattern timeouts
        checkPatternTimeout(systemState);
        checkPatternTimeout(activityState);
        
        // Update FastLED display
        FastLED.show();
    } else {
        // In stealth mode or LEDs disabled, turn off all LEDs
        systemLED[0] = CRGB::Black;
        activityLED[0] = CRGB::Black;
        FastLED.show();
    }
    
    // Update buzzer sequences (check stealth mode and buzzer enable)
    if (!stealthMode && buzzerEnabled) {
        updateBuzzer();
    }
}

void FeedbackController::on(uint8_t pin, CRGB color) {
    LEDState& state = getLEDState(pin);
    state.pattern = LEDState::SOLID;
    state.isActive = true;
    state.currentState = true;
    state.color = color;
}

void FeedbackController::off(uint8_t pin) {
    LEDState& state = getLEDState(pin);
    state.pattern = LEDState::OFF;
    state.isActive = false;
    state.currentState = false;
    state.color = CRGB::Black;
}

void FeedbackController::blink(uint8_t pin, uint16_t interval, CRGB color) {
    LEDState& state = getLEDState(pin);
    state.pattern = LEDState::BLINK;
    state.interval = interval;
    state.isActive = true;
    state.color = color;
    state.lastUpdate = millis();
}

void FeedbackController::pulse(uint8_t pin, uint16_t speed, CRGB color) {
    LEDState& state = getLEDState(pin);
    state.pattern = LEDState::PULSE;
    state.speed = speed;
    state.isActive = true;
    state.color = color;
    state.brightness = 0;
    state.fadeDirection = true;
    state.lastUpdate = millis();
}

void FeedbackController::setSetupMode() {
    pulse(Pins::SYSTEM_LED, 50, CRGB::Blue);  // Pulsing Blue
    Serial.println("[FeedbackController] Setup Mode - Pulsing Blue");
}

void FeedbackController::setConnecting() {
    blink(Pins::SYSTEM_LED, 500, CRGB::Yellow);  // Blinking Yellow
    Serial.println("[FeedbackController] Connecting Mode - Blinking Yellow");
}

void FeedbackController::setSuccess() {
    on(Pins::SYSTEM_LED, CRGB::Green);  // Solid Green
    
    // Set auto-timeout for 5 seconds
    systemState.patternStartTime = millis();
    systemState.timeoutDuration = Settings::SUCCESS_TIMEOUT;
    
    Serial.println("[FeedbackController] Success Mode - Solid Green (5s auto-timeout)");
}

void FeedbackController::playNote(uint16_t frequency, uint16_t duration) {
    if (frequency > 0) {
        ledcWriteTone(BUZZER_CHANNEL, frequency);
    } else {
        ledcWrite(BUZZER_CHANNEL, 0);  // Rest/silence
    }
    
    buzzerPlaying = true;
    buzzerStartTime = millis();
    // For single notes, we'll handle the duration in update()
}

void FeedbackController::playSuccessChime() {
    static BuzzerNote successSequence[] = {
        {NOTE_C5, 200},  // C5 for 200ms - pleasant ascending
        {NOTE_REST, 50}, // Short pause
        {NOTE_G5, 300}   // G5 for 300ms - triumphant finish
    };
    playNoteSequence(successSequence, 3);
    if (!stealthMode) {
        Serial.println("[FeedbackController] Playing Success Chime - Medieval Victory!");
    }
}

void FeedbackController::playFailureTone() {
    static BuzzerNote failureSequence[] = {
        {NOTE_G4, 200},  // G4 for 200ms - low descending
        {NOTE_REST, 50}, // Short pause  
        {NOTE_C4, 300}   // C4 for 300ms - somber finish
    };
    playNoteSequence(failureSequence, 3);
    if (!stealthMode) {
        Serial.println("[FeedbackController] Playing Failure Tone - Medieval Lament");
    }
}

void FeedbackController::playResetChime() {
    static BuzzerNote resetSequence[] = {
        {NOTE_A4, 150}   // Single A4 beep for 150ms - confirmation
    };
    playNoteSequence(resetSequence, 1);
    if (!stealthMode) {
        Serial.println("[FeedbackController] Playing Reset Chime - Medieval Acknowledgment");
    }
}

// Configuration Methods
void FeedbackController::setBrightness(uint8_t brightness) {
    ledBrightness = constrain(brightness, Settings::LED::MIN_BRIGHTNESS, Settings::LED::MAX_BRIGHTNESS);
    FastLED.setBrightness(ledBrightness);
    Serial.printf("[FeedbackController] Brightness set to %d\n", ledBrightness);
}

void FeedbackController::setStealthMode(bool enabled) {
    stealthMode = enabled;
    if (enabled) {
        // Turn off all LEDs and stop buzzer immediately
        systemLED[0] = CRGB::Black;
        activityLED[0] = CRGB::Black;
        FastLED.show();
        stopBuzzer();
        Serial.println("[FeedbackController] Stealth Mode ENABLED");
    } else {
        Serial.println("[FeedbackController] Stealth Mode DISABLED");
    }
}

void FeedbackController::enableBuzzer(bool enabled) {
    buzzerEnabled = enabled;
    if (!enabled) {
        stopBuzzer();
    }
    Serial.printf("[FeedbackController] Buzzer %s\n", enabled ? "ENABLED" : "DISABLED");
}

void FeedbackController::enableLEDs(bool enabled) {
    ledsEnabled = enabled;
    if (!enabled) {
        // Turn off all LEDs immediately
        systemLED[0] = CRGB::Black;
        activityLED[0] = CRGB::Black;
        FastLED.show();
    }
    Serial.printf("[FeedbackController] LEDs %s\n", enabled ? "ENABLED" : "DISABLED");
}

bool FeedbackController::isBuzzerPlaying() const {
    return buzzerPlaying;
}

bool FeedbackController::isLEDActive(uint8_t pin) const {
    const LEDState& state = (pin == Pins::SYSTEM_LED) ? systemState : activityState;
    return state.isActive;
}

bool FeedbackController::isStealthMode() const {
    return stealthMode;
}

// Private Helper Methods

void FeedbackController::checkPatternTimeout(LEDState& ledState) {
    // Check if pattern has a timeout and if it has expired
    if (ledState.timeoutDuration > 0 && ledState.isActive) {
        unsigned long currentTime = millis();
        if (currentTime - ledState.patternStartTime >= ledState.timeoutDuration) {
            // Timeout expired, turn off LED
            ledState.pattern = LEDState::OFF;
            ledState.isActive = false;
            ledState.timeoutDuration = 0;
            Serial.println("[FeedbackController] Pattern auto-timeout - LED turned off");
        }
    }
}

void FeedbackController::updateLED(uint8_t pin, LEDState& ledState, CRGB* ledArray) {
    if (!ledState.isActive) return;
    
    unsigned long currentTime = millis();
    
    switch (ledState.pattern) {
        case LEDState::OFF:
            ledArray[0] = CRGB::Black;
            ledState.isActive = false;
            break;
            
        case LEDState::SOLID:
            ledArray[0] = ledState.color;
            break;
            
        case LEDState::BLINK:
            if (currentTime - ledState.lastUpdate >= ledState.interval) {
                ledState.currentState = !ledState.currentState;
                ledArray[0] = ledState.currentState ? ledState.color : CRGB::Black;
                ledState.lastUpdate = currentTime;
            }
            break;
            
        case LEDState::PULSE:
            if (currentTime - ledState.lastUpdate >= ledState.speed) {
                if (ledState.fadeDirection) {
                    ledState.brightness += 5;
                    if (ledState.brightness >= 255) {
                        ledState.brightness = 255;
                        ledState.fadeDirection = false;
                    }
                } else {
                    ledState.brightness -= 5;
                    if (ledState.brightness <= 10) {
                        ledState.brightness = 10;
                        ledState.fadeDirection = true;
                    }
                }
                
                // Apply brightness to color
                ledArray[0] = ledState.color;
                ledArray[0].nscale8(ledState.brightness);
                ledState.lastUpdate = currentTime;
            }
            break;
    }
}

void FeedbackController::updateBuzzer() {
    if (!buzzerPlaying || !currentSequence) return;
    
    unsigned long currentTime = millis();
    
    // Check if current note duration has elapsed
    if (currentNoteIndex < sequenceLength) {
        BuzzerNote& currentNote = currentSequence[currentNoteIndex];
        
        if (currentTime - noteStartTime >= currentNote.duration) {
            currentNoteIndex++;
            
            if (currentNoteIndex < sequenceLength) {
                // Play next note in sequence
                BuzzerNote& nextNote = currentSequence[currentNoteIndex];
                if (nextNote.frequency > 0) {
                    ledcWriteTone(BUZZER_CHANNEL, nextNote.frequency);
                } else {
                    ledcWrite(BUZZER_CHANNEL, 0);  // Rest/silence
                }
                noteStartTime = currentTime;
            } else {
                // Sequence finished
                stopBuzzer();
            }
        }
    }
}

void FeedbackController::playNoteSequence(BuzzerNote* sequence, uint8_t length) {
    currentSequence = sequence;
    sequenceLength = length;
    currentNoteIndex = 0;
    buzzerPlaying = true;
    noteStartTime = millis();
    
    // Start playing first note
    if (length > 0) {
        BuzzerNote& firstNote = sequence[0];
        if (firstNote.frequency > 0) {
            ledcWriteTone(BUZZER_CHANNEL, firstNote.frequency);
        } else {
            ledcWrite(BUZZER_CHANNEL, 0);
        }
    }
}

void FeedbackController::stopBuzzer() {
    buzzerPlaying = false;
    currentSequence = nullptr;
    sequenceLength = 0;
    currentNoteIndex = 0;
    ledcWrite(BUZZER_CHANNEL, 0);  // Turn off buzzer
}

FeedbackController::LEDState& FeedbackController::getLEDState(uint8_t pin) {
    if (pin == Pins::SYSTEM_LED) {
        return systemState;
    } else {
        return activityState;  // Default to activity LED
    }
}
