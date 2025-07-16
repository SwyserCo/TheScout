#include <Arduino.h>
#include <unity.h>
#include "utility/LEDController.h"

LEDController ledController;

void setUp() { ledController.begin(); }
void tearDown() {}

void test_system_led_states() {
    ledController.setSystemLED(SystemLEDState::RED);
    // No direct getter, but you can check pin state if needed
    ledController.setSystemLED(SystemLEDState::GREEN);
    ledController.setSystemLED(SystemLEDState::OFF);
}

void test_activity_led_states() {
    ledController.setActivityLED(ActivityLEDState::ON);
    ledController.setActivityLED(ActivityLEDState::FLASHING);
    ledController.setActivityLED(ActivityLEDState::OFF);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_system_led_states);
    RUN_TEST(test_activity_led_states);
    UNITY_END();
}

void loop() {}
