#include "utility/MQTTHandler.h"

// Home Assistant discovery prefixes
constexpr char HA_DISCOVERY_PREFIX[] = "homeassistant";
constexpr char HA_DEVICE_CLASS_TEMP[] = "temperature";
constexpr char HA_DEVICE_CLASS_HUMID[] = "humidity";
constexpr char HA_DEVICE_CLASS_PRESS[] = "pressure";
constexpr char HA_DEVICE_CLASS_ILLUM[] = "illuminance";
constexpr char HA_DEVICE_CLASS_MOTION[] = "motion";
constexpr char HA_DEVICE_CLASS_SOUND[] = "sound";

MQTTHandler::MQTTHandler() : client(espClient), lastReconnectAttempt(0), commandCallback(nullptr), thresholdsCallback(nullptr) {}

void MQTTHandler::begin(const String& deviceId) {
    this->deviceId = std::string(deviceId.c_str());
    client.setServer(Config::MQTT_SERVER, Config::MQTT_PORT);
    client.setCallback([this](char* topic, uint8_t* payload, unsigned int length) {
        this->handleCallback(topic, payload, length);
    });
    
    setupTopics();
    lastReconnectAttempt = 0;
    publishDiscoveryConfig();
}

void MQTTHandler::setupTopics() {
    String base = String(Config::Topics::BASE) + String(deviceId.c_str()) + "/";
    
    statusTopic = std::string((base + Config::Topics::STATUS).c_str());
    motionTopic = std::string((base + Config::Topics::MOTION).c_str());
    environmentTopic = std::string((base + Config::Topics::ENVIRONMENT).c_str());
    lightTopic = std::string((base + Config::Topics::LIGHT).c_str());
    soundTopic = std::string((base + Config::Topics::SOUND).c_str());
    tamperTopic = std::string((base + Config::Topics::TAMPER).c_str());
    commandTopic = std::string((base + Config::Topics::COMMAND).c_str());
    responseTopic = std::string((base + Config::Topics::RESPONSE).c_str());
    thresholdsTopic = std::string((base + Config::Topics::THRESHOLDS).c_str());
    thresholdsSetTopic = std::string((base + Config::Topics::THRESHOLDS_SET).c_str());
    alarmTopic = std::string((base + Config::Topics::ALARM).c_str());
}

void MQTTHandler::update() {
    if (!client.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > Config::MQTT_RECONNECT_DELAY) {
            lastReconnectAttempt = now;
            if (reconnect()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        client.loop();
    }
}

bool MQTTHandler::reconnect() {
    if (client.connect(deviceId.c_str(), Config::MQTT_USER, Config::MQTT_PASSWORD)) {
        // Subscribe to command topics
        client.subscribe(commandTopic.c_str());
        client.subscribe(thresholdsSetTopic.c_str());
        
        // Publish online status
        publishStatus("online");
        
        return true;
    }
    return false;
}

void MQTTHandler::setCommandCallback(CommandCallback callback) {
    commandCallback = callback;
}

void MQTTHandler::setThresholdsCallback(ThresholdsCallback callback) {
    thresholdsCallback = callback;
}

void MQTTHandler::publishDiscoveryConfig() {
    if (!client.connected()) return;

    StaticJsonDocument<512> doc;
    String topic;
    
    // Temperature Sensor
    topic = String(HA_DISCOVERY_PREFIX) + "/sensor/" + String(deviceId.c_str()) + "/temperature/config";
    doc.clear();
    String tempName = String(deviceId.c_str()) + " Temperature";
    doc["name"] = tempName.c_str();
    doc["device_class"] = HA_DEVICE_CLASS_TEMP;
    doc["unit_of_measurement"] = "°C";
    doc["state_topic"] = environmentTopic.c_str();
    doc["value_template"] = "{{ value_json.temperature }}";
    String tempId = String(deviceId.c_str()) + "_temperature";
    doc["unique_id"] = tempId.c_str();
    publishJson(topic.c_str(), doc);
    
    // Humidity Sensor
    topic = String(HA_DISCOVERY_PREFIX) + "/sensor/" + String(deviceId.c_str()) + "/humidity/config";
    doc.clear();
    String humName = String(deviceId.c_str()) + " Humidity";
    doc["name"] = humName.c_str();
    doc["device_class"] = HA_DEVICE_CLASS_HUMID;
    doc["unit_of_measurement"] = "%";
    doc["state_topic"] = environmentTopic.c_str();
    doc["value_template"] = "{{ value_json.humidity }}";
    String humId = String(deviceId.c_str()) + "_humidity";
    doc["unique_id"] = humId.c_str();
    publishJson(topic.c_str(), doc);
    
    // Light Sensor
    topic = String(HA_DISCOVERY_PREFIX) + "/sensor/" + String(deviceId.c_str()) + "/light/config";
    doc.clear();
    String lightName = String(deviceId.c_str()) + " Light";
    doc["name"] = lightName.c_str();
    doc["device_class"] = HA_DEVICE_CLASS_ILLUM;
    doc["unit_of_measurement"] = "lx";
    doc["state_topic"] = lightTopic.c_str();
    doc["value_template"] = "{{ value_json.lux }}";
    String lightId = String(deviceId.c_str()) + "_light";
    doc["unique_id"] = lightId.c_str();
    publishJson(topic.c_str(), doc);
    
    // Motion Sensor
    topic = String(HA_DISCOVERY_PREFIX) + "/binary_sensor/" + String(deviceId.c_str()) + "/motion/config";
    doc.clear();
    String motionName = String(deviceId.c_str()) + " Motion";
    doc["name"] = motionName.c_str();
    doc["device_class"] = HA_DEVICE_CLASS_MOTION;
    doc["state_topic"] = motionTopic.c_str();
    doc["value_template"] = "{{ value_json.presence }}";
    String motionId = String(deviceId.c_str()) + "_motion";
    doc["unique_id"] = motionId.c_str();
    publishJson(topic.c_str(), doc);
    
    // Sound Level
    topic = String(HA_DISCOVERY_PREFIX) + "/sensor/" + String(deviceId.c_str()) + "/sound/config";
    doc.clear();
    String soundName = String(deviceId.c_str()) + " Sound Level";
    doc["name"] = soundName.c_str();
    doc["device_class"] = HA_DEVICE_CLASS_SOUND;
    doc["unit_of_measurement"] = "dB";
    doc["state_topic"] = soundTopic.c_str();
    doc["value_template"] = "{{ value_json.level }}";
    String soundId = String(deviceId.c_str()) + "_sound";
    doc["unique_id"] = soundId.c_str();
    publishJson(topic.c_str(), doc);
}

bool MQTTHandler::publishJson(const char* topic, JsonDocument& doc) {
    String output;
    serializeJson(doc, output);
    return client.publish(topic, output.c_str(), true);  // Retain discovery messages
}

void MQTTHandler::handleCallback(char* topic, uint8_t* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
        return;
    }
    
    if (strcmp(topic, commandTopic.c_str()) == 0) {
        if (commandCallback) {
            commandCallback(doc);
        }
    } else if (strcmp(topic, thresholdsSetTopic.c_str()) == 0) {
        if (thresholdsCallback) {
            thresholdsCallback(doc);
        }
    }
}

bool MQTTHandler::publishEnvironment(float temp, float humidity, float pressure) {
    StaticJsonDocument<200> doc;
    doc["temperature"] = temp;
    doc["humidity"] = humidity;
    doc["pressure"] = pressure;
    return publishJson(environmentTopic.c_str(), doc);
}

bool MQTTHandler::publishLight(float lux) {
    StaticJsonDocument<128> doc;
    doc["lux"] = lux;
    return publishJson(lightTopic.c_str(), doc);
}

bool MQTTHandler::publishMotion(uint8_t presence, uint16_t distance) {
    StaticJsonDocument<128> doc;
    doc["presence"] = presence > 0 ? "ON" : "OFF";
    doc["distance"] = distance;
    return publishJson(motionTopic.c_str(), doc);
}

bool MQTTHandler::publishSound(float rms, float peak) {
    StaticJsonDocument<128> doc;
    doc["level"] = rms;
    doc["peak"] = peak;
    return publishJson(soundTopic.c_str(), doc);
}

bool MQTTHandler::publishTamper(bool tampered) {
    StaticJsonDocument<128> doc;
    doc["tampered"] = tampered;
    return publishJson(tamperTopic.c_str(), doc);
}

bool MQTTHandler::publishStatus(const char* status) {
    StaticJsonDocument<128> doc;
    doc["status"] = status;
    doc["timestamp"] = millis();
    return publishJson(statusTopic.c_str(), doc);
}

bool MQTTHandler::publishAlarm(bool triggered, int reason) {
    StaticJsonDocument<128> doc;
    doc["triggered"] = triggered;
    doc["reason"] = reason;
    return publishJson(alarmTopic.c_str(), doc);
}
