#include <unity.h>
#include "mocks/arduino/VEML7700.h"
#include "test_helpers.h"

// Mock I2C communication
uint16_t mockLightLevel = 0;
class MockWire {
public:
    static void beginTransmission(uint8_t addr) {}
    static uint8_t endTransmission() { return 0; }
    static void write(uint8_t data) {}
    static void requestFrom(uint8_t addr, uint8_t len) {}
    static uint8_t read() { 
        return (mockLightLevel >> 8) & 0xFF; // Return high byte first
    }
} Wire;

void setUp(void) {
    mockLightLevel = 0;
}

void tearDown(void) {
}

void test_veml7700_init() {
    VEML7700 sensor;
    TEST_ASSERT_TRUE(sensor.begin());
}

void test_veml7700_read_zero() {
    VEML7700 sensor;
    sensor.begin();
    mockLightLevel = 0;
    
    float lux = sensor.getLux();
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, lux);
}

void test_veml7700_read_normal() {
    VEML7700 sensor;
    sensor.begin();
    mockLightLevel = 10000; // Simulate ~100 lux
    
    float lux = sensor.getLux();
    TEST_ASSERT_FLOAT_WITHIN(1.0, 100.0, lux);
}

void test_veml7700_read_bright() {
    VEML7700 sensor;
    sensor.begin();
    mockLightLevel = 65000; // Simulate very bright light
    
    float lux = sensor.getLux();
    TEST_ASSERT_TRUE(lux > 1000.0);
}

void test_veml7700_auto_range() {
    VEML7700 sensor;
    sensor.begin();
    
    // Test low light
    mockLightLevel = 100;
    float lowLux = sensor.getLux();
    
    // Test bright light
    mockLightLevel = 60000;
    float highLux = sensor.getLux();
    
    TEST_ASSERT_TRUE(highLux > lowLux);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_veml7700_init);
    RUN_TEST(test_veml7700_read_zero);
    RUN_TEST(test_veml7700_read_normal);
    RUN_TEST(test_veml7700_read_bright);
    RUN_TEST(test_veml7700_auto_range);
    return UNITY_END();
}
