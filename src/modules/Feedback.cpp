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
    // Set pin mode first
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    
    // Initialize LEDC for tone generation
    ledcSetup(0, 1000, 8); // Channel 0, 1kHz base frequency, 8-bit resolution
    ledcAttachPin(pin, 0);  // Attach pin to channel 0
    ledcWrite(0, 0);        // Start with no output
    
    Serial.println("BuzzerController: LEDC initialized for pin " + String(pin));
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
                ledcWrite(0, 0); // Ensure LEDC channel is off for rest
            } else if (currentMelody[currentNote].frequency > 0) {
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
    
    // Use tone() for proper PWM buzzer sound with safety check
    if (frequency > 0) {
        tone(pin, frequency);
    }
}

void BuzzerController::stop() {
    active = false;
    playingMelody = false;
    noTone(pin);
    ledcWrite(0, 0); // Ensure LEDC channel is off
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
        ledcWrite(0, 0); // Ensure LEDC channel is off for rest
    } else if (melody[0].frequency > 0) {
        tone(pin, melody[0].frequency);
    }
}

void BuzzerController::playConnectSuccess() {
    // Medieval victory bells - fast celebratory pattern
    static const Note successMelody[] = {
        {Config::NOTE_D4, Config::NOTE_SIXTEENTH},  // Quick ornament
        {Config::NOTE_F4, Config::NOTE_SIXTEENTH},  // Building up
        {Config::NOTE_A4, Config::NOTE_EIGHTH},     // Rising
        {Config::NOTE_D5, Config::NOTE_QUARTER},    // Victory peak!
        {Config::NOTE_A4, Config::NOTE_EIGHTH},     // Echo down
        {Config::NOTE_F4, Config::NOTE_EIGHTH},     // And
        {Config::NOTE_D4, Config::NOTE_QUARTER}     // Resolve
    };
    playMelody(successMelody, 7);
}

void BuzzerController::playSuccessChime() {
    // Prevent rapid repeated calls
    static unsigned long lastSuccessChime = 0;
    if (millis() - lastSuccessChime < 2000) {
        Serial.println("BuzzerController: Success chime blocked (too soon after last one)");
        return;
    }
    lastSuccessChime = millis();
    
    // Simple 2-tone success chime (per PRD)
    static const Note successChime[] = {
        {Config::NOTE_C5, Config::NOTE_EIGHTH},     // First chime
        {Config::NOTE_D5, Config::NOTE_QUARTER}     // Second chime (higher)
    };
    
    Serial.println("BuzzerController: Playing success chime");
    playMelody(successChime, 2);
}

void BuzzerController::playConnectFailed() {
    // Medieval "alarm bell" pattern - urgent repetitive warning
    static const Note failMelody[] = {
        {Config::NOTE_A3, Config::NOTE_EIGHTH},     // Ding-
        {Config::NOTE_REST, Config::NOTE_SIXTEENTH}, // Quick break
        {Config::NOTE_A3, Config::NOTE_EIGHTH},     // ding-
        {Config::NOTE_REST, Config::NOTE_SIXTEENTH}, // Quick break  
        {Config::NOTE_F3, Config::NOTE_QUARTER},    // DONG!
        {Config::NOTE_REST, Config::NOTE_EIGHTH},   // Brief pause
        {Config::NOTE_D3, Config::NOTE_HALF}        // Final low warning toll
    };
    playMelody(failMelody, 7);
}

void BuzzerController::playStartup() {
    // Medieval herald fanfare - quick trumpet-like call
    static const Note startupMelody[] = {
        {Config::NOTE_C4, Config::NOTE_EIGHTH},     // Ta-
        {Config::NOTE_G4, Config::NOTE_EIGHTH},     // ta-
        {Config::NOTE_C5, Config::NOTE_QUARTER},    // TAAAA!
        {Config::NOTE_REST, Config::NOTE_SIXTEENTH},
        {Config::NOTE_G4, Config::NOTE_EIGHTH},     // Quick echo
        {Config::NOTE_C5, Config::NOTE_HALF}        // Final sustain
    };
    playMelody(startupMelody, 6);
}

void BuzzerController::playResetChime() {
    // Medieval "ceremony complete" chime - dignified and final
    static const Note resetMelody[] = {
        {Config::NOTE_F4, Config::NOTE_QUARTER},    // Ceremonial
        {Config::NOTE_A4, Config::NOTE_QUARTER},    // rising
        {Config::NOTE_C5, Config::NOTE_HALF},       // to completion
        {Config::NOTE_REST, Config::NOTE_EIGHTH},   // Brief pause
        {Config::NOTE_F4, Config::NOTE_EIGHTH},     // Echo
        {Config::NOTE_C5, Config::NOTE_HALF}        // Final confirmation
    };
    playMelody(resetMelody, 6);
}
