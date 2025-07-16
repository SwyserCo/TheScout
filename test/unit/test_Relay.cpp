#include <Arduino.h>
#include <unity.h>
#include "sensors/Relay.h"

Relay relay;

void setUp() {
    relay.begin();
}

void tearDown() {
    // Nothing needed
}

void test_initial_state() {
    TEST_ASSERT_EQUAL(RelayState::OFF, relay.getState());
}

void test_set_on() {
    relay.setState(RelayState::ON);
    TEST_ASSERT_EQUAL(RelayState::ON, relay.getState());
}

void test_set_off() {
    relay.setState(RelayState::OFF);
    TEST_ASSERT_EQUAL(RelayState::OFF, relay.getState());
}

void test_toggle() {
    relay.setState(RelayState::OFF);
    relay.toggle();
    TEST_ASSERT_EQUAL(RelayState::ON, relay.getState());
    relay.toggle();
    TEST_ASSERT_EQUAL(RelayState::OFF, relay.getState());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_set_on);
    RUN_TEST(test_set_off);
    RUN_TEST(test_toggle);
    UNITY_END();
}

void loop() {
    // Not used
}
