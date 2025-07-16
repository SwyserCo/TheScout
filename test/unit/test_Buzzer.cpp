#include <Arduino.h>
#include <unity.h>
#include "utility/Buzzer.h"

Buzzer buzzer;

void setUp() { buzzer.begin(); }
void tearDown() {}

void test_play_chimes() {
    buzzer.playChime(BuzzerChime::SUCCESS);
    buzzer.playChime(BuzzerChime::FAILURE);
    buzzer.playChime(BuzzerChime::TIMEOUT);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_play_chimes);
    UNITY_END();
}

void loop() {}
