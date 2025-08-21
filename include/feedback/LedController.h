#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// The Scout - LED Controller
// Manages individual LEDs with non-blocking state machine patterns

// Color Palette - Guardian Medieval Theme
#define GUARDIAN_GREEN   0x00FF00   // System operational
#define GUARDIAN_RED     0xFF0000   // Error/Alert
#define GUARDIAN_BLUE    0x0000FF   // Activity/Processing  
#define GUARDIAN_ORANGE  0xFF8000   // Warning/Transition
#define GUARDIAN_OFF     0x000000   // LED off

// Pixel Names for clarity
#define PIXEL_SYSTEM     0          // System status LED
#define PIXEL_ACTIVITY   1          // Activity status LED

// LED Pattern Types
enum LedPattern {
    PATTERN_OFF,
    PATTERN_SOLID,
    PATTERN_BLINK,
    PATTERN_PULSE
};

// LED State for non-blocking operation
struct LedState {
    LedPattern pattern;
    uint32_t color;
    uint16_t interval;
    uint16_t speed;
    unsigned long lastUpdate;
    uint8_t brightness;
    bool isOn;
    int pulseDirection;
};

class LedController {
private:
    Adafruit_NeoPixel systemLed;
    Adafruit_NeoPixel activityLed;
    
    LedState systemState;
    LedState activityState;
    
    uint8_t globalBrightness;
    bool stealthMode;
    
    // Helper methods
    void updatePattern(Adafruit_NeoPixel& strip, LedState& state, uint8_t pixelIndex);
    uint32_t dimColor(uint32_t color, uint8_t brightness);
    uint8_t calculatePulseBrightness(LedState& state);
    
public:
    LedController();
    
    // Core lifecycle methods
    void begin();
    void update();  // Non-blocking - call in main loop
    
    // Global settings
    void setBrightness(uint8_t brightness);
    void setStealthMode(bool enabled);
    uint8_t getBrightness() const;
    bool getStealthMode() const;
    
    // Direct pixel control
    void setPixelColor(uint8_t pixelIndex, uint32_t color);
    void setSystemLed(uint32_t color);
    void setActivityLed(uint32_t color);
    
    // Pattern methods (non-blocking)
    void blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval = 500);
    void pulsePixel(uint8_t pixelIndex, uint32_t color, uint16_t speed = 50);
    void blinkSystem(uint32_t color, uint16_t interval = 500);
    void blinkActivity(uint32_t color, uint16_t interval = 500);
    void pulseSystem(uint32_t color, uint16_t speed = 50);
    void pulseActivity(uint32_t color, uint16_t speed = 50);
    
    // Utility methods
    void turnOff();
    void turnOffPixel(uint8_t pixelIndex);
    void turnOffSystem();
    void turnOffActivity();
    
    // Status methods
    bool isSystemOn() const;
    bool isActivityOn() const;
};

#endif // LED_CONTROLLER_H
