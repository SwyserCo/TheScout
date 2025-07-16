#include <unity.h>
#include "../include/sensors/MicrophoneSensor.h"
#include "test_helpers.h"

// Mock analogRead values
int mockAnalogValue = 0;

void setUp(void) {
    _millis = 0;
    mockAnalogValue = 0;
}

void tearDown(void) {
}

void test_microphone_init() {
    MicrophoneSensor mic;
    TEST_ASSERT_TRUE(mic.begin());
}

void test_microphone_silence() {
    MicrophoneSensor mic;
    mic.begin();
    mockAnalogValue = 512; // ADC midpoint, no sound
    
    mic.read();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, mic.getRMSLevel());
    TEST_ASSERT_EQUAL_INT(0, mic.getPeakLevel());
}

void test_microphone_constant_sound() {
    MicrophoneSensor mic;
    mic.begin();
    mockAnalogValue = 1024; // Constant loud sound
    
    // Multiple reads to build RMS
    for(int i = 0; i < 10; i++) {
        mic.read();
        advance_time(10);
    }
    
    float rms = mic.getRMSLevel();
    int peak = mic.getPeakLevel();
    
    TEST_ASSERT_TRUE(rms > 0.0);
    TEST_ASSERT_TRUE(peak > 0);
    TEST_ASSERT_TRUE(peak >= rms);
}

void test_microphone_peak_detection() {
    MicrophoneSensor mic;
    mic.begin();
    
    // Simulate a sudden loud sound
    mockAnalogValue = 4095; // Max ADC value
    mic.read();
    advance_time(10);
    
    // Return to silence
    mockAnalogValue = 512;
    for(int i = 0; i < 10; i++) {
        mic.read();
        advance_time(10);
    }
    
    int peak = mic.getPeakLevel();
    TEST_ASSERT_TRUE(peak > 0);
}

void test_microphone_decay() {
    MicrophoneSensor mic;
    mic.begin();
    
    // Simulate loud sound
    mockAnalogValue = 4095;
    mic.read();
    int initialPeak = mic.getPeakLevel();
    
    // Let peak decay
    mockAnalogValue = 512;
    advance_time(1000);
    mic.read();
    int decayedPeak = mic.getPeakLevel();
    
    TEST_ASSERT_TRUE(decayedPeak < initialPeak);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_microphone_init);
    RUN_TEST(test_microphone_silence);
    RUN_TEST(test_microphone_constant_sound);
    RUN_TEST(test_microphone_peak_detection);
    RUN_TEST(test_microphone_decay);
    return UNITY_END();
}
