#include <unity.h>
#include "mocks/arduino/WiFiSetup.h"
#include "test_helpers.h"

// Mock WiFi class
struct MockWiFiClass {
    static bool isConnected;
    static String SSID;
    static String password;
    static void begin(const char* ssid, const char* pass) {
        SSID = ssid;
        password = pass;
    }
    static bool status() {
        return isConnected;
    }
};

bool MockWiFiClass::isConnected = false;
String MockWiFiClass::SSID;
String MockWiFiClass::password;

void setUp(void) {
    _millis = 0;
    MockWiFiClass::isConnected = false;
    MockWiFiClass::SSID = "";
    MockWiFiClass::password = "";
}

void tearDown(void) {
}

void test_wifi_initial_state() {
    WiFiSetup wifi;
    wifi.begin();
    TEST_ASSERT_FALSE(wifi.isConnected());
}

void test_wifi_connection_success() {
    WiFiSetup wifi;
    wifi.begin();
    
    // Simulate successful connection
    MockWiFiClass::isConnected = true;
    wifi.handle();
    TEST_ASSERT_TRUE(wifi.isConnected());
}

void test_wifi_connection_failure() {
    WiFiSetup wifi;
    wifi.begin();
    
    // Simulate connection failure
    MockWiFiClass::isConnected = false;
    wifi.handle();
    TEST_ASSERT_FALSE(wifi.isConnected());
}

void test_wifi_device_id() {
    WiFiSetup wifi;
    wifi.begin();
    
    // Device ID should not be empty
    String deviceId = wifi.getDeviceID();
    TEST_ASSERT_NOT_EQUAL(0, deviceId.length());
}

void test_wifi_reconnection() {
    WiFiSetup wifi;
    wifi.begin();
    
    // Initial connection
    MockWiFiClass::isConnected = true;
    wifi.handle();
    TEST_ASSERT_TRUE(wifi.isConnected());
    
    // Connection lost
    MockWiFiClass::isConnected = false;
    wifi.handle();
    TEST_ASSERT_FALSE(wifi.isConnected());
    
    // Reconnected
    MockWiFiClass::isConnected = true;
    wifi.handle();
    TEST_ASSERT_TRUE(wifi.isConnected());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_initial_state);
    RUN_TEST(test_wifi_connection_success);
    RUN_TEST(test_wifi_connection_failure);
    RUN_TEST(test_wifi_device_id);
    RUN_TEST(test_wifi_reconnection);
    return UNITY_END();
}
