#include <unity.h>
#include <Arduino.h>
#include "../include/Config.h"
#include "mocks/arduino/Relay.h"
#include "test_helpers.h"

// Mock digital write state
int pinStates[64] = {0};
void digitalWrite(uint8_t pin, uint8_t val) {
    pinStates[pin] = val;
}

void pinMode(uint8_t pin, uint8_t mode) {}

void setUp(void) {
    _millis = 0;
    memset(pinStates, 0, sizeof(pinStates));
}

void tearDown(void) {
}

void test_relay_init() {
    Relay relay;
    relay.begin();
    TEST_ASSERT_EQUAL(LOW, pinStates[Config::RELAY_PIN]);
}

void test_relay_on() {
    Relay relay;
    relay.begin();
    relay.on();
    TEST_ASSERT_EQUAL(HIGH, pinStates[Config::RELAY_PIN]);
}

void test_relay_off() {
    Relay relay;
    relay.begin();
    relay.on();
    relay.off();
    TEST_ASSERT_EQUAL(LOW, pinStates[Config::RELAY_PIN]);
}

void test_relay_toggle() {
    Relay relay;
    relay.begin();
    
    // Initial state is off
    TEST_ASSERT_EQUAL(LOW, pinStates[Config::RELAY_PIN]);
    
    // Toggle on
    relay.toggle();
    TEST_ASSERT_EQUAL(HIGH, pinStates[Config::RELAY_PIN]);
    
    // Toggle off
    relay.toggle();
    TEST_ASSERT_EQUAL(LOW, pinStates[Config::RELAY_PIN]);
}

void test_relay_get_state() {
    Relay relay;
    relay.begin();
    
    TEST_ASSERT_FALSE(relay.isOn());
    
    relay.on();
    TEST_ASSERT_TRUE(relay.isOn());
    
    relay.off();
    TEST_ASSERT_FALSE(relay.isOn());
}

void test_relay_pulse() {
    Relay relay;
    relay.begin();
    
    relay.pulse(1000); // 1 second pulse
    TEST_ASSERT_TRUE(relay.isOn());
    
    advance_time(500);
    relay.update();
    TEST_ASSERT_TRUE(relay.isOn());
    
    advance_time(600); // Total 1.1 seconds
    relay.update();
    TEST_ASSERT_FALSE(relay.isOn());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_relay_init);
    RUN_TEST(test_relay_on);
    RUN_TEST(test_relay_off);
    RUN_TEST(test_relay_toggle);
    RUN_TEST(test_relay_get_state);
    RUN_TEST(test_relay_pulse);
    return UNITY_END();
}
