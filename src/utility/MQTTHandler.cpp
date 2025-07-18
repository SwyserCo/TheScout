#include "utility/MQTTHandler.h"
#include "Config.h"

MQTTHandler* MQTTHandler::instance = nullptr;

MQTTHandler::MQTTHandler() : _client(_wifiClient), _lastReconnectAttempt(0) {
    instance = this;
}

void MQTTHandler::begin(const String& deviceId) {
    _deviceId = deviceId;
    _baseTopic = "homeassistant/";
    
    _client.setServer(Config::MQTT_BROKER, Config::MQTT_PORT);
    _client.setCallback(staticMessageCallback);
    _client.setKeepAlive(Config::MQTT_KEEP_ALIVE);
}

void MQTTHandler::loop() {
    if (!_client.connected()) {
        uint32_t now = millis();
        if (now - _lastReconnectAttempt > Config::MQTT_RECONNECT_DELAY) {
            _lastReconnectAttempt = now;
            if (reconnect()) {
                _lastReconnectAttempt = 0;
            }
        }
    } else {
        _client.loop();
    }
}

bool MQTTHandler::isConnected() const {
    // Cast away const to call connected() method
    return const_cast<PubSubClient*>(&_client)->connected();
}

void MQTTHandler::publishSensorData(const String& sensor, const DynamicJsonDocument& data) {
    if (!_client.connected()) return;
    
    String topic = "guardian/scout/" + _deviceId + "/" + sensor;
    String payload;
    serializeJson(data, payload);
    
    _client.publish(topic.c_str(), payload.c_str(), true);
}

void MQTTHandler::publishAlarmState(const String& state) {
    if (!_client.connected()) return;
    
    String topic = "guardian/scout/" + _deviceId + "/alarm/state";
    _client.publish(topic.c_str(), state.c_str(), true);
}

void MQTTHandler::publishDeviceState(const String& state) {
    if (!_client.connected()) return;
    
    String topic = "guardian/scout/" + _deviceId + "/status";
    _client.publish(topic.c_str(), state.c_str(), true);
}

void MQTTHandler::publishRelayState(bool state) {
    if (!_client.connected()) return;
    
    String topic = "guardian/scout/" + _deviceId + "/relay/state";
    _client.publish(topic.c_str(), state ? "ON" : "OFF", true);
}

void MQTTHandler::publishDiscoveryConfig() {
    if (!_client.connected()) return;
    
    // Temperature sensor
    publishDiscoverySensor("Temperature", "temperature", 
        "guardian/scout/" + _deviceId + "/environment", "°C");
    
    // Humidity sensor
    publishDiscoverySensor("Humidity", "humidity", 
        "guardian/scout/" + _deviceId + "/environment", "%");
    
    // Pressure sensor
    publishDiscoverySensor("Pressure", "pressure", 
        "guardian/scout/" + _deviceId + "/environment", "hPa");
    
    // Light sensor
    publishDiscoverySensor("Light", "illuminance", 
        "guardian/scout/" + _deviceId + "/light", "lx");
    
    // Motion sensor
    publishDiscoveryBinarySensor("Motion", "motion", 
        "guardian/scout/" + _deviceId + "/motion");
    
    // Tamper sensor
    publishDiscoveryBinarySensor("Tamper", "tamper", 
        "guardian/scout/" + _deviceId + "/tamper");
    
    // Relay switch
    publishDiscoverySwitch("Relay", 
        "guardian/scout/" + _deviceId + "/relay/state",
        "guardian/scout/" + _deviceId + "/relay/set");
    
    // Alarm panel
    publishDiscoveryAlarm("Alarm", 
        "guardian/scout/" + _deviceId + "/alarm/state",
        "guardian/scout/" + _deviceId + "/alarm/set");
}

void MQTTHandler::setAlarmCallback(std::function<void(const String&)> callback) {
    _alarmCallback = callback;
}

void MQTTHandler::setRelayCallback(std::function<void(bool)> callback) {
    _relayCallback = callback;
}

void MQTTHandler::setThresholdCallback(std::function<void(const String&, float)> callback) {
    _thresholdCallback = callback;
}

bool MQTTHandler::reconnect() {
    if (_client.connect(_deviceId.c_str(), Config::MQTT_USER, Config::MQTT_PASSWORD)) {
        Serial.println("MQTT connected");
        
        // Subscribe to command topics
        String alarmTopic = "guardian/scout/" + _deviceId + "/alarm/set";
        String relayTopic = "guardian/scout/" + _deviceId + "/relay/set";
        String thresholdTopic = "guardian/scout/" + _deviceId + "/thresholds/set";
        
        _client.subscribe(alarmTopic.c_str());
        _client.subscribe(relayTopic.c_str());
        _client.subscribe(thresholdTopic.c_str());
        
        publishDeviceState("online");
        publishDiscoveryConfig();
        
        return true;
    }
    
    Serial.print("MQTT connection failed, rc=");
    Serial.println(_client.state());
    return false;
}

void MQTTHandler::onMessage(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    String topicStr = String(topic);
    
    if (topicStr.endsWith("/alarm/set") && _alarmCallback) {
        _alarmCallback(message);
    } else if (topicStr.endsWith("/relay/set") && _relayCallback) {
        _relayCallback(message == "ON");
    } else if (topicStr.endsWith("/thresholds/set") && _thresholdCallback) {
        // Parse JSON threshold message
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);
        if (!error) {
            if (doc.containsKey("motion_threshold")) {
                _thresholdCallback("motion", doc["motion_threshold"]);
            }
        }
    }
}

void MQTTHandler::publishDiscoveryDevice(const String& component, const String& name, const String& deviceClass) {
    DynamicJsonDocument doc(1024);
    doc["name"] = name;
    doc["unique_id"] = _deviceId + "_" + name.substring(0, 10);
    doc["device"]["identifiers"][0] = _deviceId;
    doc["device"]["name"] = _deviceId;
    doc["device"]["model"] = "The Scout";
    doc["device"]["manufacturer"] = "Guardian Security";
    
    if (deviceClass.length() > 0) {
        doc["device_class"] = deviceClass;
    }
    
    String topic = _baseTopic + component + "/" + _deviceId + "/" + name + "/config";
    String payload;
    serializeJson(doc, payload);
    
    _client.publish(topic.c_str(), payload.c_str(), true);
}

void MQTTHandler::publishDiscoveryBinarySensor(const String& name, const String& deviceClass, const String& stateTopic) {
    DynamicJsonDocument doc(1024);
    doc["name"] = name;
    doc["unique_id"] = _deviceId + "_" + name.substring(0, 10);
    doc["device_class"] = deviceClass;
    doc["state_topic"] = stateTopic;
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    
    doc["device"]["identifiers"][0] = _deviceId;
    doc["device"]["name"] = _deviceId;
    doc["device"]["model"] = "The Scout";
    doc["device"]["manufacturer"] = "Guardian Security";
    
    String topic = _baseTopic + "binary_sensor/" + _deviceId + "/" + name + "/config";
    String payload;
    serializeJson(doc, payload);
    
    _client.publish(topic.c_str(), payload.c_str(), true);
}

void MQTTHandler::publishDiscoverySensor(const String& name, const String& deviceClass, const String& stateTopic, const String& unit) {
    DynamicJsonDocument doc(1024);
    doc["name"] = name;
    doc["unique_id"] = _deviceId + "_" + name.substring(0, 10);
    doc["device_class"] = deviceClass;
    doc["state_topic"] = stateTopic;
    doc["value_template"] = "{{ value_json." + name.substring(0, 10) + " }}";
    
    if (unit.length() > 0) {
        doc["unit_of_measurement"] = unit;
    }
    
    doc["device"]["identifiers"][0] = _deviceId;
    doc["device"]["name"] = _deviceId;
    doc["device"]["model"] = "The Scout";
    doc["device"]["manufacturer"] = "Guardian Security";
    
    String topic = _baseTopic + "sensor/" + _deviceId + "/" + name + "/config";
    String payload;
    serializeJson(doc, payload);
    
    _client.publish(topic.c_str(), payload.c_str(), true);
}

void MQTTHandler::publishDiscoverySwitch(const String& name, const String& stateTopic, const String& commandTopic) {
    DynamicJsonDocument doc(1024);
    doc["name"] = name;
    doc["unique_id"] = _deviceId + "_" + name.substring(0, 10);
    doc["state_topic"] = stateTopic;
    doc["command_topic"] = commandTopic;
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    
    doc["device"]["identifiers"][0] = _deviceId;
    doc["device"]["name"] = _deviceId;
    doc["device"]["model"] = "The Scout";
    doc["device"]["manufacturer"] = "Guardian Security";
    
    String topic = _baseTopic + "switch/" + _deviceId + "/" + name + "/config";
    String payload;
    serializeJson(doc, payload);
    
    _client.publish(topic.c_str(), payload.c_str(), true);
}

void MQTTHandler::publishDiscoveryAlarm(const String& name, const String& stateTopic, const String& commandTopic) {
    DynamicJsonDocument doc(1024);
    doc["name"] = name;
    doc["unique_id"] = _deviceId + "_alarm";
    doc["state_topic"] = stateTopic;
    doc["command_topic"] = commandTopic;
    doc["supported_features"] = 3; // ARM_AWAY and DISARM
    doc["code_arm_required"] = false;
    doc["code_disarm_required"] = false;
    
    doc["device"]["identifiers"][0] = _deviceId;
    doc["device"]["name"] = _deviceId;
    doc["device"]["model"] = "The Scout";
    doc["device"]["manufacturer"] = "Guardian Security";
    
    String topic = _baseTopic + "alarm_control_panel/" + _deviceId + "/" + name + "/config";
    String payload;
    serializeJson(doc, payload);
    
    _client.publish(topic.c_str(), payload.c_str(), true);
}

void MQTTHandler::staticMessageCallback(char* topic, byte* payload, unsigned int length) {
    if (instance) {
        instance->onMessage(topic, payload, length);
    }
}
