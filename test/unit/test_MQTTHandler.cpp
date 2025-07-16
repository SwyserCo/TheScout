#include <Arduino.h>
#include <unity.h>
#include "utility/MQTTHandler.h"
#include "sensors/Relay.h"
#include "sensors/SensorManager.h"

MQTTHandler mqttHandler;
Relay relay;
SensorManager sensorManager;

void setUp() {
    mqttHandler.setRelayPtr(&relay);
    mqttHandler.setSensorManagerPtr(&sensorManager);
}
void tearDown() {}

void test_publish_sensor_data() {
    // This is a stub; actual publish would require a running broker
    mqttHandler.publishSensorData();
}

void test_publish_alarm_state() {
    mqttHandler.publishAlarmState(true);
    mqttHandler.publishAlarmState(false);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_publish_sensor_data);
    RUN_TEST(test_publish_alarm_state);
    UNITY_END();
}

void loop() {}
