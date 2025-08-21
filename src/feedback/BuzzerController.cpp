#include "feedback/BuzzerController.h"
#include "config/Settings.h"

// Predefined medieval chime sequences - Each tells a story

// Success - Triumphant ascending fanfare (like a knight's victory)
const ToneNote BuzzerController::successChime[] = {
    {NOTE_C4, TONE_SHORT},
    {NOTE_E4, TONE_SHORT},
    {NOTE_G4, TONE_MEDIUM},
    {NOTE_C5, TONE_MEDIUM}
};

// Failure - Descending minor progression (somber, like a defeated quest)
const ToneNote BuzzerController::failureChime[] = {
    {NOTE_G4, TONE_SHORT},
    {NOTE_F4, TONE_SHORT},
    {NOTE_E4, TONE_MEDIUM},
    {NOTE_C4, TONE_MEDIUM}
};

// Reset - Sacred bell sequence (like monastery bells calling to prayer)
const ToneNote BuzzerController::resetChime[] = {
    {NOTE_C5, TONE_SHORT},
    {0, TONE_VERY_SHORT},   // Silence
    {NOTE_G4, TONE_SHORT},
    {0, TONE_VERY_SHORT},   // Silence
    {NOTE_C4, TONE_MEDIUM}
};

// Pending - Mysterious questioning melody (like a riddle being posed)
const ToneNote BuzzerController::pendingChime[] = {
    {NOTE_G3, TONE_SHORT},
    {NOTE_C4, TONE_VERY_SHORT},
    {NOTE_E4, TONE_VERY_SHORT},
    {NOTE_G4, TONE_MEDIUM}
};

// Question - Inquisitive rising then falling (like a curious bard)
const ToneNote BuzzerController::questionChime[] = {
    {NOTE_C4, TONE_VERY_SHORT},
    {NOTE_E4, TONE_VERY_SHORT},
    {NOTE_G4, TONE_SHORT},
    {NOTE_F4, TONE_SHORT}
};

// Warning - Urgent but controlled (like a watchman's alert)
const ToneNote BuzzerController::warningChime[] = {
    {NOTE_A4, TONE_VERY_SHORT},
    {NOTE_A4, TONE_VERY_SHORT},
    {0, TONE_VERY_SHORT},   // Brief silence
    {NOTE_A4, TONE_SHORT}
};

// Alert - Rapid battle horn sequence (urgent danger!)
const ToneNote BuzzerController::alertChime[] = {
    {NOTE_C5, TONE_VERY_SHORT},
    {NOTE_C5, TONE_VERY_SHORT},
    {NOTE_C5, TONE_VERY_SHORT},
    {0, TONE_VERY_SHORT},   // Brief silence
    {NOTE_G4, TONE_SHORT},
    {NOTE_C5, TONE_SHORT}
};

// Welcome - Warm greeting (like a tavern keeper's welcome)
const ToneNote BuzzerController::welcomeChime[] = {
    {NOTE_C4, TONE_SHORT},
    {NOTE_G4, TONE_VERY_SHORT},
    {NOTE_E4, TONE_VERY_SHORT},
    {NOTE_C4, TONE_VERY_SHORT},
    {NOTE_G4, TONE_MEDIUM}
};

// Farewell - Gentle descending blessing (like a peaceful departure)
const ToneNote BuzzerController::farewellChime[] = {
    {NOTE_C5, TONE_SHORT},
    {NOTE_G4, TONE_SHORT},
    {NOTE_E4, TONE_SHORT},
    {NOTE_C4, TONE_MEDIUM}
};

BuzzerController::BuzzerController(uint8_t buzzerPin, uint8_t channel)
    : pin(buzzerPin), ledcChannel(channel), stealthMode(false),
      currentState(BUZZER_IDLE), currentSequence(nullptr),
      sequenceLength(0), currentNoteIndex(0), noteStartTime(0),
      totalPlayTime(0), lastStopTime(0), thermalProtectionActive(false) {
}

void BuzzerController::begin() {
    Serial.printf("[BUZZER] Initializing Buzzer Controller on pin %d, channel %d...\n", pin, ledcChannel);
    
    // Configure LEDC for buzzer PWM with proper frequency
    ledcSetup(ledcChannel, 1000, BUZZER_LEDC_RESOLUTION);  // Start with 1kHz base frequency
    ledcAttachPin(pin, ledcChannel);
    
    // Start silent
    ledcWrite(ledcChannel, 0);
    
    // Test buzzer with a brief, low-power beep (thermal protection)
    Serial.println("[BUZZER] Testing buzzer with safe initialization beep...");
    ledcWriteTone(ledcChannel, 1000);
    ledcWrite(ledcChannel, BUZZER_DUTY_CYCLE);  // Use safe duty cycle
    // Note: We'll turn off the beep in the first update() call
    
    Serial.printf("[BUZZER] Controller initialized - Stealth: %s (Thermal Protection: ON)\n", stealthMode ? "ON" : "OFF");
}

void BuzzerController::update() {
    if (stealthMode || currentState == BUZZER_IDLE) {
        return;
    }
    
    unsigned long currentTime = millis();
    
    // Thermal protection check
    if (thermalProtectionActive) {
        if (currentTime - lastStopTime < BUZZER_COOLDOWN_TIME) {
            return; // Still in cooldown period
        } else {
            thermalProtectionActive = false;
            totalPlayTime = 0;
            Serial.println("[BUZZER] Thermal protection cooldown complete");
        }
    }
    
    // Check if we've been playing too long
    if (totalPlayTime > BUZZER_MAX_CONTINUOUS_TIME) {
        Serial.println("[BUZZER] Thermal protection activated - buzzer needs to cool down");
        stopNote();
        currentState = BUZZER_IDLE;
        thermalProtectionActive = true;
        lastStopTime = currentTime;
        return;
    }
    
    if (currentState == BUZZER_PLAYING_SEQUENCE) {
        // Check if current note duration has elapsed
        if (currentTime - noteStartTime >= currentSequence[currentNoteIndex].duration) {
            totalPlayTime += currentSequence[currentNoteIndex].duration;
            stopNote();
            currentNoteIndex++;
            
            // Check if sequence is complete
            if (currentNoteIndex >= sequenceLength) {
                currentState = BUZZER_IDLE;
                Serial.println("[BUZZER] Sequence complete");
                return;
            }
            
            // Non-blocking gap between notes - just start the next note
            playNote(currentSequence[currentNoteIndex].frequency, 
                    currentSequence[currentNoteIndex].duration);
        }
    }
}

void BuzzerController::playNote(uint16_t frequency, uint16_t duration) {
    if (stealthMode) return;
    
    if (frequency > 0) {
        // Use ledcChangeFrequency for dynamic frequency changes
        ledcChangeFrequency(ledcChannel, frequency, BUZZER_LEDC_RESOLUTION);
        ledcWrite(ledcChannel, BUZZER_DUTY_CYCLE);  // Safer 12.5% duty cycle to prevent overheating
        Serial.printf("[BUZZER] Playing note: %dHz for %dms\n", frequency, duration);
    } else {
        ledcWrite(ledcChannel, 0);
        Serial.println("[BUZZER] Playing silence");
    }
    
    noteStartTime = millis();
}

void BuzzerController::stopNote() {
    ledcWrite(ledcChannel, 0);
}

void BuzzerController::startSequence(const ToneNote* sequence, uint8_t length) {
    if (stealthMode) return;
    
    currentSequence = sequence;
    sequenceLength = length;
    currentNoteIndex = 0;
    currentState = BUZZER_PLAYING_SEQUENCE;
    
    // Start playing first note
    playNote(sequence[0].frequency, sequence[0].duration);
    
    Serial.printf("[BUZZER] Starting sequence with %d notes\n", length);
}

void BuzzerController::setStealthMode(bool enabled) {
    stealthMode = enabled;
    if (enabled) {
        stop();
    }
    Serial.printf("[BUZZER] Stealth mode: %s\n", enabled ? "ENABLED" : "DISABLED");
}

bool BuzzerController::getStealthMode() const {
    return stealthMode;
}

void BuzzerController::playSuccessChime() {
    Serial.println("[BUZZER] Playing success chime - Triumphant victory!");
    startSequence(successChime, sizeof(successChime) / sizeof(ToneNote));
}

void BuzzerController::playFailureTone() {
    Serial.println("[BUZZER] Playing failure tone - Quest unsuccessful");
    startSequence(failureChime, sizeof(failureChime) / sizeof(ToneNote));
}

void BuzzerController::playResetChime() {
    Serial.println("[BUZZER] Playing reset chime - Sacred bells toll");
    startSequence(resetChime, sizeof(resetChime) / sizeof(ToneNote));
}

void BuzzerController::playPendingChime() {
    Serial.println("[BUZZER] Playing pending chime - Mystery unfolds");
    startSequence(pendingChime, sizeof(pendingChime) / sizeof(ToneNote));
}

void BuzzerController::playQuestionChime() {
    Serial.println("[BUZZER] Playing question chime - Curious inquiry");
    startSequence(questionChime, sizeof(questionChime) / sizeof(ToneNote));
}

void BuzzerController::playWarningChime() {
    Serial.println("[BUZZER] Playing warning chime - Watchman's alert");
    startSequence(warningChime, sizeof(warningChime) / sizeof(ToneNote));
}

void BuzzerController::playAlertChime() {
    Serial.println("[BUZZER] Playing alert chime - Battle horn sounds!");
    startSequence(alertChime, sizeof(alertChime) / sizeof(ToneNote));
}

void BuzzerController::playWelcomeChime() {
    Serial.println("[BUZZER] Playing welcome chime - Tavern keeper's greeting");
    startSequence(welcomeChime, sizeof(welcomeChime) / sizeof(ToneNote));
}

void BuzzerController::playFarewellChime() {
    Serial.println("[BUZZER] Playing farewell chime - Peaceful blessing");
    startSequence(farewellChime, sizeof(farewellChime) / sizeof(ToneNote));
}

void BuzzerController::playTone(uint16_t frequency, uint16_t duration) {
    if (stealthMode) return;
    
    ToneNote singleNote = {frequency, duration};
    startSequence(&singleNote, 1);
}

void BuzzerController::playCustomSequence(const ToneNote* sequence, uint8_t length) {
    startSequence(sequence, length);
}

void BuzzerController::stop() {
    currentState = BUZZER_IDLE;
    stopNote();
    Serial.println("[BUZZER] Stopped");
}

bool BuzzerController::isPlaying() const {
    return currentState == BUZZER_PLAYING_SEQUENCE;
}
