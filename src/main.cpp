#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "modules/Feedback.h"
#include "modules/WiFiManager.h"
#include "modules/FactoryResetHandler.h"

// Create LED arrays for FastLED
CRGB systemLEDs[Config::NUM_LEDS_PER_STRIP];
CRGB activityLEDs[Config::NUM_LEDS_PER_STRIP];

// Create feedback module instances with LED array references
LEDController systemLED(Config::SYSTEM_LED_PIN, systemLEDs);
LEDController activityLED(Config::ACTIVITY_LED_PIN, activityLEDs);
BuzzerController buzzer(Config::BUZZER_PIN);

// Create WiFi manager
WiFiManager wifiManager(&systemLED, &buzzer);

// Create factory reset handler
FactoryResetHandler factoryReset(&systemLED, &buzzer);

void setup() {
    // Initialize serial communication
    Serial.begin(Config::SERIAL_BAUD);
    Serial.println("TheScout - Phase 2 + 5: WiFi & Factory Reset");
    
    // Initialize FastLED with error checking
    FastLED.addLeds<WS2812B, Config::SYSTEM_LED_PIN, GRB>(systemLEDs, Config::NUM_LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, Config::ACTIVITY_LED_PIN, GRB>(activityLEDs, Config::NUM_LEDS_PER_STRIP);
    FastLED.setBrightness(Config::LED_BRIGHTNESS);
    
    // Initialize feedback modules
    systemLED.begin();
    activityLED.begin();
    buzzer.begin();
    
    Serial.println("Hardware initialized successfully");
    
    // Initialize factory reset handler first (higher priority)
    factoryReset.begin();
    
    // Initialize WiFi manager (will handle its own feedback)
    wifiManager.begin();
    
    Serial.println("TheScout Guard System Ready - Phase 2 + 5");
}

void loop() {
    // Update factory reset handler first (highest priority)
    factoryReset.update();
    
    // Always update buzzer for factory reset chimes
    buzzer.update();
    
    // Only update other modules if factory reset is not in progress
    if (!factoryReset.isResetInProgress()) {
        // Update all modules
        systemLED.update();
        activityLED.update();
        wifiManager.update();
    } else {
        // During factory reset, let factory reset handler control the LED
        systemLED.update();
    }
    
    // Small delay to prevent watchdog issues and reduce CPU usage
    delay(Config::LOOP_DELAY_MS);
}