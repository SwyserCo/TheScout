#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "utility/LEDController.h"
#include "utility/Buzzer.h"
#include "sensors/AccelerometerSensor.h"
#include "sensors/LD2420Sensor.h"

enum class AlarmState {
    DISARMED,
    ARMED,
    TRIGGERED,
    SILENCED
};

class AlarmSystem {
public:
    AlarmSystem(LEDController& activityLED, Buzzer& buzzer,
                AccelerometerSensor& accel, LD2420Sensor& presence);
    
    void begin();
    void update();
    void setState(AlarmState state);
    AlarmState getState() const;
    String getStateString() const;
    void silence();
    void reset();
    
    // Sensor threshold setters
    void setTamperThreshold(float threshold);
    void setPresenceThreshold(uint16_t threshold);
    
private:
    LEDController& _activityLED;
    Buzzer& _buzzer;
    AccelerometerSensor& _accel;
    LD2420Sensor& _presence;
    
    AlarmState _state;
    AlarmState _previousState;
    uint32_t _lastUpdate;
    bool _alarmTriggered;
    String _triggerReason;
    
    static constexpr uint32_t UPDATE_INTERVAL = 100; // 0.1 seconds
    
    void checkSensors();
    void handleStateChange();
    void triggerAlarm(const String& reason);
};
