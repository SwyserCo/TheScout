#pragma once

#include "sensors/Relay.h"

class SensorManager;

class MQTTHandler {
public:
    void begin();
    void handle();
    void setRelayPtr(Relay* relay);
    void setSensorManagerPtr(SensorManager* sensorManager);
    void publishSensorData();
    void publishAlarmState(bool triggered);

private:
    Relay* relayPtr = nullptr;
    SensorManager* sensorManagerPtr = nullptr;
    void handleRelayCommand(const char* payload);
    void publishRelayState();
};
