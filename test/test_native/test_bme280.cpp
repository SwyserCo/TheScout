#include <unity.h>
#include <Arduino.h>
#include <Wire.h>
#include "sensors/BME280Sensor.h"

void setUp(void) {
    _millis = 0;
}

void tearDown(void) {
}

void test_bme280_initialization(void) {
    BME280Sensor sensor;
    bool success = sensor.begin();
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_TRUE(sensor.isReady());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bme280_initialization);
    return UNITY_END();
}
