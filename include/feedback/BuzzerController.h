#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

#include <Arduino.h>

// The Scout - Buzzer Controller
// Medieval themed audio feedback using ESP32 LEDC PWM

// Musical Notes (Hz) - Medieval Theme
#define NOTE_C3     131
#define NOTE_D3     147
#define NOTE_E3     165
#define NOTE_F3     175
#define NOTE_G3     196
#define NOTE_A3     220
#define NOTE_B3     247
#define NOTE_C4     262
#define NOTE_D4     294
#define NOTE_E4     330
#define NOTE_F4     349
#define NOTE_G4     392
#define NOTE_A4     440
#define NOTE_B4     494
#define NOTE_C5     523
#define NOTE_D5     587
#define NOTE_E5     659
#define NOTE_F5     698
#define NOTE_G5     784
#define NOTE_A5     880

// Tone durations (ms) - Medieval timing (shorter for better UX)
#define TONE_VERY_SHORT 100
#define TONE_SHORT      200
#define TONE_MEDIUM     350
#define TONE_LONG       500
#define TONE_VERY_LONG  600
#define TONE_SUSTAIN    750

// Buzzer States for non-blocking operation
enum BuzzerState {
    BUZZER_IDLE,
    BUZZER_PLAYING_SEQUENCE
};

// Note structure for sequences
struct ToneNote {
    uint16_t frequency;
    uint16_t duration;
};

// Predefined chime sequences
enum ChimeType {
    CHIME_SUCCESS,
    CHIME_FAILURE,
    CHIME_RESET,
    CHIME_PENDING,
    CHIME_QUESTION,
    CHIME_WARNING,
    CHIME_ALERT,
    CHIME_WELCOME,
    CHIME_FAREWELL,
    CHIME_CUSTOM
};

class BuzzerController {
private:
    uint8_t ledcChannel;
    uint8_t pin;
    bool stealthMode;
    
    // State machine variables
    BuzzerState currentState;
    const ToneNote* currentSequence;
    uint8_t sequenceLength;
    uint8_t currentNoteIndex;
    unsigned long noteStartTime;
    
    // Thermal protection variables
    unsigned long totalPlayTime;
    unsigned long lastStopTime;
    bool thermalProtectionActive;
    
    // Predefined sequences
    static const ToneNote successChime[];
    static const ToneNote failureChime[];
    static const ToneNote resetChime[];
    static const ToneNote pendingChime[];
    static const ToneNote questionChime[];
    static const ToneNote warningChime[];
    static const ToneNote alertChime[];
    static const ToneNote welcomeChime[];
    static const ToneNote farewellChime[];
    
    // Helper methods
    void playNote(uint16_t frequency, uint16_t duration);
    void stopNote();
    void startSequence(const ToneNote* sequence, uint8_t length);
    
public:
    BuzzerController(uint8_t buzzerPin, uint8_t channel = 0);
    
    // Core lifecycle methods
    void begin();
    void update();  // Non-blocking - call in main loop
    
    // Settings
    void setStealthMode(bool enabled);
    bool getStealthMode() const;
    
    // Predefined chimes (non-blocking)
    void playSuccessChime();
    void playFailureTone();
    void playResetChime();
    void playPendingChime();
    void playQuestionChime();
    void playWarningChime();
    void playAlertChime();
    void playWelcomeChime();
    void playFarewellChime();
    
    // Simple tone methods
    void playTone(uint16_t frequency, uint16_t duration);
    void playCustomSequence(const ToneNote* sequence, uint8_t length);
    
    // Utility
    void stop();
    bool isPlaying() const;
};

#endif // BUZZER_CONTROLLER_H
