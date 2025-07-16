#pragma once

#include <Arduino.h>

enum class AlarmState : uint8_t {
    DISARMED = 0,
    ARMED = 1,
    TRIGGERED = 2
};

class MQTTHandler;

class TamperingProtection {
public:
    void begin();
    void handle();
    void arm();
    void disarm();
    AlarmState getState() const;
    void triggerAlarm();
    void setMQTTHandler(MQTTHandler* handler);

private:
    AlarmState state = AlarmState::DISARMED;
    unsigned long alarmTriggeredTime = 0;
    const unsigned long alarmDuration = 5000; // ms
    MQTTHandler* mqttHandler = nullptr;
};
