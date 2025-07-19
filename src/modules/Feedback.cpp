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
BuzzerController::BuzzerController(uint8_t buzzerPin) : 
    pin(buzzerPin), 
    startTime(0), 
    duration(0), 
    active(false), 
    enabled(true),
    currentMelody(nullptr),
    melodyLength(0),
    currentNote(0),
    playingMelody(false),
    noteStartTime(0) {}

void BuzzerController::begin() {
    // No need to set pinMode for tone() - it handles PWM setup automatically
    // Just ensure it's off initially
    noTone(pin);
}

void BuzzerController::update() {
    if (!enabled) return;
    
    unsigned long currentTime = millis();
    
    // Handle melody playback
    if (playingMelody && currentMelody) {
        if (currentTime - noteStartTime >= currentMelody[currentNote].duration) {
            currentNote++;
            
            if (currentNote >= melodyLength) {
                // Melody finished
                playingMelody = false;
                active = false;
                noTone(pin);
                return;
            }
            
            // Play next note
            noteStartTime = currentTime;
            if (currentMelody[currentNote].frequency == Config::NOTE_REST) {
                noTone(pin);
            } else {
                tone(pin, currentMelody[currentNote].frequency);
            }
        }
        return;
    }
    
    // Handle single beep
    if (active && currentTime - startTime >= duration) {
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
    playingMelody = false;
    noTone(pin);
}

void BuzzerController::enable() {
    enabled = true;
}

void BuzzerController::disable() {
    enabled = false;
    stop();
}

bool BuzzerController::isActive() {
    return active || playingMelody;
}

void BuzzerController::playMelody(const Note* melody, uint8_t length) {
    if (!enabled || !melody || length == 0) return;
    
    currentMelody = melody;
    melodyLength = length;
    currentNote = 0;
    playingMelody = true;
    active = true;
    noteStartTime = millis();
    
    // Start playing first note
    if (melody[0].frequency == Config::NOTE_REST) {
        noTone(pin);
    } else {
        tone(pin, melody[0].frequency);
    }
}

void BuzzerController::playConnectSuccess() {
    // Ascending chime: C4-E4-G4-C5 (success melody)
    static const Note successMelody[] = {
        {Config::NOTE_C4, Config::NOTE_EIGHTH},
        {Config::NOTE_E4, Config::NOTE_EIGHTH},
        {Config::NOTE_G4, Config::NOTE_EIGHTH},
        {Config::NOTE_C5, Config::NOTE_QUARTER}
    };
    playMelody(successMelody, 4);
}

void BuzzerController::playConnectFailed() {
    // Descending chime: C5-A4-F4-D4 (failure melody)
    static const Note failMelody[] = {
        {Config::NOTE_C5, Config::NOTE_EIGHTH},
        {Config::NOTE_A4, Config::NOTE_EIGHTH},
        {Config::NOTE_F4, Config::NOTE_EIGHTH},
        {Config::NOTE_D4, Config::NOTE_QUARTER}
    };
    playMelody(failMelody, 4);
}

void BuzzerController::playStartup() {
    // Startup chime: C4-G4-C5-REST-C5 (friendly greeting)
    static const Note startupMelody[] = {
        {Config::NOTE_C4, Config::NOTE_EIGHTH},
        {Config::NOTE_G4, Config::NOTE_EIGHTH},
        {Config::NOTE_C5, Config::NOTE_QUARTER},
        {Config::NOTE_REST, Config::NOTE_EIGHTH},
        {Config::NOTE_C5, Config::NOTE_EIGHTH}
    };
    playMelody(startupMelody, 5);
}
