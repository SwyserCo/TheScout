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

// Buzzer Controller Class - Non-blocking buzzer control  
class BuzzerController {
private:
    uint8_t pin;
    unsigned long startTime;
    unsigned long duration;
    bool active;
    bool enabled;
    
public:
    BuzzerController(uint8_t buzzerPin);
    void begin();
    void update();           // Non-blocking update - call in loop()
    void beep(unsigned long beepDuration, uint16_t frequency = 1000);
    void stop();
    void enable();
    void disable();
    bool isActive();
};
