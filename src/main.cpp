#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "modules/Feedback.h"
#include "modules/WiFiManager.h"

// Create LED arrays for FastLED
CRGB systemLEDs[Config::NUM_LEDS_PER_STRIP];
CRGB activityLEDs[Config::NUM_LEDS_PER_STRIP];

// Create feedback module instances with LED array references
LEDController systemLED(Config::SYSTEM_LED_PIN, systemLEDs);
LEDController activityLED(Config::ACTIVITY_LED_PIN, activityLEDs);
BuzzerController buzzer(Config::BUZZER_PIN);

// Create WiFi manager
WiFiManager wifiManager(&systemLED, &buzzer);

void setup() {
    // Initialize serial communication
    Serial.begin(Config::SERIAL_BAUD);
    Serial.println("TheScout - Phase 2: WiFi Setup & Connectivity");
    
    // Initialize FastLED with error checking
    FastLED.addLeds<WS2812B, Config::SYSTEM_LED_PIN, GRB>(systemLEDs, Config::NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, Config::ACTIVITY_LED_PIN, GRB>(activityLEDs, Config::NUM_LEDS_PER_STRIP);
    FastLED.setBrightness(Config::LED_BRIGHTNESS);
    
    // Initialize feedback modules
    systemLED.begin();
    activityLED.begin();
    buzzer.begin();
    
    Serial.println("Hardware initialized successfully");
    
    // Play startup chime
    buzzer.playStartup();
    
    // Initialize WiFi manager
    wifiManager.begin();
    
    Serial.println("TheScout Guard System Ready - Phase 2");
}

void loop() {
    // Update all modules
    systemLED.update();
    activityLED.update();
    buzzer.update();
    wifiManager.update();
    
    // Small delay to prevent watchdog issues and reduce CPU usage
    delay(Config::LOOP_DELAY_MS);
}