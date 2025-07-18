#include "utility/AlarmSystem.h"
#include "Config.h"

AlarmSystem::AlarmSystem(LEDController& activityLED, Buzzer& buzzer,
                         LIS2DW12Sensor& accel, LD2420Sensor& presence)
    : _activityLED(activityLED), _buzzer(buzzer), _accel(accel), 
      _presence(presence),
      _state(AlarmState::DISARMED), _previousState(AlarmState::DISARMED),
      _lastUpdate(0), _alarmTriggered(false), _triggerReason("") {
}

void AlarmSystem::begin() {
    // Initialize components
    _activityLED.begin();
    _buzzer.begin();
    
    // Set initial state
    setState(AlarmState::DISARMED);
    
    Serial.println("Alarm system initialized");
}

void AlarmSystem::update() {
    uint32_t currentTime = millis();
    
    if (currentTime - _lastUpdate >= UPDATE_INTERVAL) {
        // Update LED and buzzer
        _activityLED.update();
        _buzzer.update();
        
        // Check sensors if armed
        if (_state == AlarmState::ARMED) {
            checkSensors();
        }
        
        // Handle state changes
        if (_state != _previousState) {
            handleStateChange();
            _previousState = _state;
        }
        
        _lastUpdate = currentTime;
    }
}

void AlarmSystem::setState(AlarmState state) {
    _state = state;
    
    switch (state) {
        case AlarmState::DISARMED:
            _activityLED.setState(LEDState::OFF);
            _buzzer.stop();
            _alarmTriggered = false;
            _triggerReason = "";
            Serial.println("Alarm disarmed");
            break;
            
        case AlarmState::ARMED:
            _activityLED.setState(LEDState::ON);
            _buzzer.stop();
            _alarmTriggered = false;
            _triggerReason = "";
            Serial.println("Alarm armed");
            break;
            
        case AlarmState::TRIGGERED:
            _activityLED.setState(LEDState::FAST_BLINK);
            _buzzer.playPattern(BuzzerPattern::ALARM_SIREN);
            Serial.println("Alarm triggered: " + _triggerReason);
            break;
            
        case AlarmState::SILENCED:
            _activityLED.setState(LEDState::FAST_BLINK);
            _buzzer.stop();
            Serial.println("Alarm silenced");
            break;
    }
}

AlarmState AlarmSystem::getState() const {
    return _state;
}

String AlarmSystem::getStateString() const {
    switch (_state) {
        case AlarmState::DISARMED: return "disarmed";
        case AlarmState::ARMED: return "armed_away";
        case AlarmState::TRIGGERED: return "triggered";
        case AlarmState::SILENCED: return "silenced";
        default: return "unknown";
    }
}

void AlarmSystem::silence() {
    if (_state == AlarmState::TRIGGERED) {
        setState(AlarmState::SILENCED);
    }
}

void AlarmSystem::reset() {
    setState(AlarmState::DISARMED);
}

void AlarmSystem::setTamperThreshold(float threshold) {
    _accel.setTamperThreshold(threshold);
}

void AlarmSystem::setPresenceThreshold(uint16_t threshold) {
    _presence.setPresenceThreshold(threshold);
}

void AlarmSystem::checkSensors() {
    if (_state != AlarmState::ARMED) return;
    
    // Check accelerometer for tamper
    if (_accel.checkTamper()) {
        triggerAlarm("Tamper detected");
        return;
    }
    
    // Check presence sensor
    if (_presence.checkPresence()) {
        triggerAlarm("Presence detected");
        return;
    }
}

void AlarmSystem::handleStateChange() {
    // State change logic already handled in setState()
}

void AlarmSystem::triggerAlarm(const String& reason) {
    if (_state == AlarmState::ARMED) {
        _triggerReason = reason;
        _alarmTriggered = true;
        setState(AlarmState::TRIGGERED);
    }
}
