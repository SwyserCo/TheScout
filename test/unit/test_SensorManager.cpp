#include <Arduino.h>
#include <unity.h>
#include "sensors/SensorManager.h"

SensorManager sensorManager;

void setUp() { sensorManager.begin(); }
void tearDown() {}

void test_sensor_defaults() {
    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.5f, sensorManager.getTemperature());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 45.0f, sensorManager.getHumidity());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 1013.25f, sensorManager.getPressure());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 123.4f, sensorManager.getAmbientLight());
    TEST_ASSERT_FALSE(sensorManager.getPresence());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 60.0f, sensorManager.getSoundLevel());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_sensor_defaults);
    UNITY_END();
}

void loop() {}
