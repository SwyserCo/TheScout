#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "modules/Feedback.h"

// Create LED arrays for FastLED
CRGB systemLEDs[Config::NUM_LEDS_PER_STRIP];
CRGB activityLEDs[Config::NUM_LEDS_PER_STRIP];

// Create feedback module instances with LED array references
LEDController systemLED(Config::SYSTEM_LED_PIN, systemLEDs);
LEDController activityLED(Config::ACTIVITY_LED_PIN, activityLEDs);
BuzzerController buzzer(Config::BUZZER_PIN);

// Demo variables
unsigned long lastDemo = 0;
uint8_t demoStep = 0;

void setup() {
    // Initialize serial communication
    Serial.begin(Config::SERIAL_BAUD);
    Serial.println("TheScout - Phase 1: Feedback Modules Test (WS2812B RGB LEDs)");
    
    // Initialize FastLED
    FastLED.addLeds<WS2812B, Config::SYSTEM_LED_PIN, GRB>(systemLEDs, Config::NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, Config::ACTIVITY_LED_PIN, GRB>(activityLEDs, Config::NUM_LEDS_PER_STRIP);
    FastLED.setBrightness(Config::LED_BRIGHTNESS); // Use config constant
    
    // Initialize feedback modules
    systemLED.begin();
    activityLED.begin();
    buzzer.begin();
    
    // Start with medieval torch-like system LED
    systemLED.setBlink(Config::LED_BLINK_SLOW, Config::Colors::TORCH_ORANGE);
    
    Serial.println("✅ Feedback modules initialized with WS2812B RGB LEDs");
    Serial.println("🏰 Demo sequence starting...");
    
    // Play startup chime to indicate device is ready
    buzzer.playStartup();
}

void loop() {
    // Update all feedback modules (non-blocking)
    systemLED.update();
    activityLED.update();
    buzzer.update();
    
    // Demo sequence every 3 seconds
    if (millis() - lastDemo >= 3000) {
        lastDemo = millis();
        
        switch (demoStep) {
            case 0:
                Serial.println("Step 1: Activity LED fast blink (blue) + Startup Chime");
                activityLED.setBlink(Config::LED_BLINK_FAST, CRGB::Blue);
                buzzer.playStartup();
                break;
                
            case 1:
                Serial.println("Step 2: Both LEDs on (red + green) + WiFi Success Chime");
                systemLED.setOn(CRGB::Red);
                activityLED.setOn(CRGB::Green);
                buzzer.playConnectSuccess();
                break;
                
            case 2:
                Serial.println("Step 3: System LED fast blink (yellow) + WiFi Failed Chime");
                systemLED.setBlink(Config::LED_BLINK_FAST, CRGB::Yellow);
                activityLED.setOff();
                buzzer.playConnectFailed();
                break;
                
            case 3:
                Serial.println("Step 4: Reset to initial state (white slow blink) + Short beep");
                systemLED.setBlink(Config::LED_BLINK_SLOW, CRGB::White);
                activityLED.setOff();
                buzzer.beep(Config::BUZZER_BEEP_SHORT, Config::BUZZER_FREQ_HIGH);
                break;
        }
        
        demoStep = (demoStep + 1) % 4;
    }
    
    // Small delay to prevent watchdog issues and reduce CPU usage
    delay(Config::LOOP_DELAY_MS);
}