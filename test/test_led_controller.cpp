#include <unity.h>
#include "../include/utility/LEDController.h"
#include "test_helpers.h"

// Mock digital/analog write functions
int pinStates[40] = {0};
void digitalWrite(uint8_t pin, uint8_t state) {
    pinStates[pin] = state;
}

void analogWrite(uint8_t pin, uint8_t value) {
    pinStates[pin] = value;
}

void pinMode(uint8_t pin, uint8_t mode) {}

void setUp(void) {
    memset(pinStates, 0, sizeof(pinStates));
    _millis = 0;
}

void tearDown(void) {
}

void test_led_initialization() {
    LEDController controller;
    controller.begin();
    
    TEST_ASSERT_EQUAL(0, pinStates[Config::STATUS_LED_PIN]);
    TEST_ASSERT_EQUAL(0, pinStates[Config::RGB_LED_R_PIN]);
    TEST_ASSERT_EQUAL(0, pinStates[Config::RGB_LED_G_PIN]);
    TEST_ASSERT_EQUAL(0, pinStates[Config::RGB_LED_B_PIN]);
}

void test_status_led_patterns() {
    LEDController controller;
    controller.begin();
    
    // Test ON pattern
    controller.setStatusPattern(LEDPattern::ON);
    controller.update();
    TEST_ASSERT_EQUAL(HIGH, pinStates[Config::STATUS_LED_PIN]);
    
    // Test OFF pattern
    controller.setStatusPattern(LEDPattern::OFF);
    controller.update();
    TEST_ASSERT_EQUAL(LOW, pinStates[Config::STATUS_LED_PIN]);
    
    // Test BLINK pattern
    controller.setStatusPattern(LEDPattern::BLINK_SLOW);
    
    controller.update();
    TEST_ASSERT_EQUAL(HIGH, pinStates[Config::STATUS_LED_PIN]);
    
    advance_time(1000);
    controller.update();
    TEST_ASSERT_EQUAL(LOW, pinStates[Config::STATUS_LED_PIN]);
}

void test_rgb_led_control() {
    LEDController controller;
    controller.begin();
    
    controller.setRGBColor(255, 128, 64);
    controller.setRGBPattern(LEDPattern::ON);
    controller.update();
    
    TEST_ASSERT_EQUAL(255, pinStates[Config::RGB_LED_R_PIN]);
    TEST_ASSERT_EQUAL(128, pinStates[Config::RGB_LED_G_PIN]);
    TEST_ASSERT_EQUAL(64, pinStates[Config::RGB_LED_B_PIN]);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_led_initialization);
    RUN_TEST(test_status_led_patterns);
    RUN_TEST(test_rgb_led_control);
    
    return UNITY_END();
}
