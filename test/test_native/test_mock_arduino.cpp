#include <unity.h>
#include "mocks/Arduino.h"

void setUp(void) {
    _millis = 0;
}

void tearDown(void) {
}

void test_millis_increments_with_delay(void) {
    uint32_t start = millis();
    delay(1000);
    TEST_ASSERT_EQUAL(1000, millis() - start);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_millis_increments_with_delay);
    UNITY_END();
    
    return 0;
}
