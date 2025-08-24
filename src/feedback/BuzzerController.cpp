#include "feedback/BuzzerController.h"
#include "config/Pins.h"

void BuzzerController::begin() {
    #ifdef DEBUG
    Serial.println("[BUZZER] Initializing BuzzerController...");
    Serial.flush();
    #endif
    
    // Initialize the buzzer pin as output
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    buzzerInitialized = true;
    
    // Play boot sound sequence - but only if not in stealth mode
    if (!stealthMode) {
        currentSound = SoundState::BOOT;
        soundStartTime = millis();
        startTone(TONE_BOOT_LOW);
    }
    
    #ifdef DEBUG
    Serial.println("[BUZZER] Initialization complete with boot sound");
    Serial.flush();
    #endif
}

void BuzzerController::update() {
    if (!buzzerInitialized || stealthMode) {
        return;
    }

    // Handle software PWM for active tones
    if (isToneActive) {
        unsigned long currentTime = micros();
        if (currentTime - lastToggle >= halfPeriod) {
            // Toggle the buzzer pin
            digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
            lastToggle = currentTime;
        }
    }

    // Handle sound state machine
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - soundStartTime;

    switch (currentSound) {
        case SoundState::BOOT:
            if (elapsed >= DURATION_BOOT) {
                stopTone();
                currentSound = SoundState::IDLE;
            }
            break;

        case SoundState::SUCCESS_NOTE1:
            if (elapsed >= DURATION_SUCCESS_SHORT) {
                stopTone();
                currentSound = SoundState::SUCCESS_GAP;
                soundStartTime = currentTime;
            }
            break;

        case SoundState::SUCCESS_GAP:
            if (elapsed >= DURATION_SUCCESS_GAP) {
                startTone(TONE_SUCCESS_HIGH);
                currentSound = SoundState::SUCCESS_NOTE2;
                soundStartTime = currentTime;
            }
            break;

        case SoundState::SUCCESS_NOTE2:
            if (elapsed >= DURATION_SUCCESS_SHORT) {
                stopTone();
                currentSound = SoundState::IDLE;
            }
            break;

        case SoundState::FAILURE:
            if (elapsed >= DURATION_FAILURE) {
                stopTone();
                currentSound = SoundState::IDLE;
            }
            break;

        case SoundState::CONFIRM:
            if (elapsed >= DURATION_CONFIRM) {
                stopTone();
                currentSound = SoundState::IDLE;
            }
            break;

        case SoundState::INTERACTION:
            if (elapsed >= DURATION_INTERACTION) {
                stopTone();
                currentSound = SoundState::IDLE;
            }
            break;

        case SoundState::IDLE:
            // Nothing to do
            break;
    }
}

void BuzzerController::setStealthMode(bool enabled) {
    stealthMode = enabled;
    
    #ifdef DEBUG
    Serial.printf("[BUZZER] Stealth mode %s\n", enabled ? "enabled" : "disabled");
    #endif
    
    // Stop any current sounds if entering stealth mode
    if (enabled && currentSound != SoundState::IDLE) {
        stopTone();
        currentSound = SoundState::IDLE;
    }
}

void BuzzerController::playSuccess() {
    if (!buzzerInitialized || stealthMode || currentSound != SoundState::IDLE) {
        return;
    }
    
    #ifdef DEBUG
    Serial.println("[BUZZER] Playing success sound");
    #endif
    
    currentSound = SoundState::SUCCESS_NOTE1;
    soundStartTime = millis();
    startTone(TONE_SUCCESS_LOW);
}

void BuzzerController::playFailure() {
    if (!buzzerInitialized || stealthMode || currentSound != SoundState::IDLE) {
        return;
    }
    
    #ifdef DEBUG
    Serial.println("[BUZZER] Playing failure sound");
    #endif
    
    currentSound = SoundState::FAILURE;
    soundStartTime = millis();
    startTone(TONE_FAILURE);
}

void BuzzerController::playConfirm() {
    if (!buzzerInitialized || stealthMode || currentSound != SoundState::IDLE) {
        return;
    }
    
    #ifdef DEBUG
    Serial.println("[BUZZER] Playing confirmation sound");
    #endif
    
    currentSound = SoundState::CONFIRM;
    soundStartTime = millis();
    startTone(TONE_CONFIRM);
}

void BuzzerController::playInteraction() {
    if (!buzzerInitialized || stealthMode || currentSound != SoundState::IDLE) {
        return;
    }
    
    #ifdef DEBUG
    Serial.println("[BUZZER] Playing interaction sound");
    #endif
    
    currentSound = SoundState::INTERACTION;
    soundStartTime = millis();
    startTone(TONE_INTERACTION);
}

void BuzzerController::startTone(uint16_t frequency) {
    if (!buzzerInitialized || stealthMode) {
        return;
    }
    
    // Ensure frequency is within a reasonable range
    if (frequency < 50 || frequency > 20000) {
        #ifdef DEBUG
        Serial.printf("[BUZZER] Warning: Invalid frequency %d Hz, skipping\n", frequency);
        #endif
        return;
    }
    
    // Store frequency for software PWM implementation
    currentFrequency = frequency;
    isToneActive = true;
    lastToggle = micros();
    
    // Calculate half period in microseconds
    halfPeriod = (1000000UL / frequency) / 2;
    
    #ifdef DEBUG
    Serial.printf("[BUZZER] Starting software PWM tone at %d Hz (half-period: %lu us)\n", frequency, halfPeriod);
    #endif
}

void BuzzerController::stopTone() {
    if (!buzzerInitialized) {
        return;
    }
    
    // Stop software PWM
    isToneActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    
    #ifdef DEBUG
    Serial.println("[BUZZER] Stopping software PWM tone");
    #endif
}
