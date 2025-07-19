#pragma once
#include <Arduino.h>
#include <FastLED.h>

// RGB LED Controller Class - Non-blocking WS2812B control
class LEDController {
private:
    uint8_t pin;
    CRGB* leds;  // Reference to LED array
    unsigned long lastToggle;
    unsigned long interval;
    bool state;
    bool enabled;
    CRGB currentColor;
    CRGB offColor;
    
public:
    LEDController(uint8_t ledPin, CRGB* ledArray);
    void begin();
    void update();           // Non-blocking update - call in loop()
    void setBlink(unsigned long blinkInterval, CRGB color = CRGB::White);
    void setColor(CRGB color);
    void setOn(CRGB color = CRGB::White);
    void setOff();
    void enable();
    void disable();
};

// Note structure for melodies
struct Note {
    uint16_t frequency;
    uint16_t duration;
};

// Buzzer Controller Class - Non-blocking buzzer control with melody support
class BuzzerController {
private:
    uint8_t pin;
    unsigned long startTime;
    unsigned long duration;
    bool active;
    bool enabled;
    
    // Melody support
    const Note* currentMelody;
    uint8_t melodyLength;
    uint8_t currentNote;
    bool playingMelody;
    unsigned long noteStartTime;
    
public:
    BuzzerController(uint8_t buzzerPin);
    void begin();
    void update();           // Non-blocking update - call in loop()
    void beep(unsigned long beepDuration, uint16_t frequency = 1000);
    void playMelody(const Note* melody, uint8_t length);
    void playConnectSuccess();   // Plays WiFi connect success chime
    void playConnectFailed();    // Plays WiFi connect failed chime
    void playStartup();          // Plays device startup chime
    void stop();
    void enable();
    void disable();
    bool isActive();
};
