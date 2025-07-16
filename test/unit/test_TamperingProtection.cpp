#include <Arduino.h>
#include <unity.h>
#include "utility/TamperingProtection.h"

TamperingProtection tp;

void setUp() { tp.begin(); }
void tearDown() {}

void test_initial_state() {
    TEST_ASSERT_EQUAL(AlarmState::DISARMED, tp.getState());
}

void test_arm_disarm() {
    tp.arm();
    TEST_ASSERT_EQUAL(AlarmState::ARMED, tp.getState());
    tp.disarm();
    TEST_ASSERT_EQUAL(AlarmState::DISARMED, tp.getState());
}

void test_trigger_alarm() {
    tp.arm();
    tp.triggerAlarm();
    TEST_ASSERT_EQUAL(AlarmState::TRIGGERED, tp.getState());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_arm_disarm);
    RUN_TEST(test_trigger_alarm);
    UNITY_END();
}

void loop() {}
