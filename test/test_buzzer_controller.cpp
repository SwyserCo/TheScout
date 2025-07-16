#include <unity.h>
#include "../include/utility/BuzzerController.h"
#include "test_helpers.h"

// Mock tone function
int currentToneFrequency = 0;
int currentToneDuration = 0;
void tone(uint8_t pin, unsigned int freq, unsigned long duration) {
    currentToneFrequency = freq;
    currentToneDuration = duration;
}

void noTone(uint8_t pin) {
    currentToneFrequency = 0;
    currentToneDuration = 0;
}

void setUp(void) {
    _millis = 0;
    currentToneFrequency = 0;
    currentToneDuration = 0;
}

void tearDown(void) {
}

void test_buzzer_initial_state() {
    BuzzerController buzzer;
    buzzer.begin();
    buzzer.update();
    TEST_ASSERT_EQUAL(0, currentToneFrequency);
}

void test_buzzer_alarm_pattern() {
    BuzzerController buzzer;
    buzzer.begin();
    buzzer.playPattern(BuzzerPattern::ALARM);
    
    // First beep should be active
    buzzer.update();
    TEST_ASSERT_NOT_EQUAL(0, currentToneFrequency);
    
    // After half period, should be silent
    advance_time(500);
    buzzer.update();
    TEST_ASSERT_EQUAL(0, currentToneFrequency);
    
    // After full period, should beep again
    advance_time(500);
    buzzer.update();
    TEST_ASSERT_NOT_EQUAL(0, currentToneFrequency);
}

void test_buzzer_chime() {
    BuzzerController buzzer;
    buzzer.begin();
    buzzer.playChime(BuzzerChime::SUCCESS);
    
    buzzer.update();
    TEST_ASSERT_NOT_EQUAL(0, currentToneFrequency);
    
    // After chime duration
    advance_time(200);
    buzzer.update();
    TEST_ASSERT_EQUAL(0, currentToneFrequency);
}

void test_buzzer_stop() {
    BuzzerController buzzer;
    buzzer.begin();
    buzzer.playPattern(BuzzerPattern::ALARM);
    buzzer.update();
    TEST_ASSERT_NOT_EQUAL(0, currentToneFrequency);
    
    buzzer.stop();
    buzzer.update();
    TEST_ASSERT_EQUAL(0, currentToneFrequency);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_buzzer_initial_state);
    RUN_TEST(test_buzzer_alarm_pattern);
    RUN_TEST(test_buzzer_chime);
    RUN_TEST(test_buzzer_stop);
    return UNITY_END();
}
