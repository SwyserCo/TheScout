#pragma once
#include "../Config.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <string>
#include <cstdint>

class MQTTHandler {
public:
    MQTTHandler() : client(espClient) {}
    
    void begin(const String& deviceId) {
        this->deviceId = deviceId.c_str();
        client.setServer(Config::MQTT_SERVER, Config::MQTT_PORT);
        client.setCallback([this](char* topic, uint8_t* payload, unsigned int length) {
            this->handleCallback(topic, payload, length);
        });
        
        // Set up topics
        std::string base = std::string(Config::Topics::BASE) + this->deviceId;
        statusTopic = base + Config::Topics::STATUS;
        motionTopic = base + Config::Topics::MOTION;
        environmentTopic = base + Config::Topics::ENVIRONMENT;
        lightTopic = base + Config::Topics::LIGHT;
        soundTopic = base + Config::Topics::SOUND;
        tamperTopic = base + Config::Topics::TAMPER;
        commandTopic = base + Config::Topics::COMMAND;
        responseTopic = base + Config::Topics::RESPONSE;
        thresholdsTopic = base + Config::Topics::THRESHOLDS;
        thresholdsSetTopic = base + Config::Topics::THRESHOLDS_SET;
        alarmTopic = base + Config::Topics::ALARM;
    }
    
    void update() {
        if (!client.connected()) {
            reconnect();
        }
        client.loop();
    }
    
    bool publishStatus(bool online) {
        StaticJsonDocument<128> doc;
        doc["status"] = online ? "online" : "offline";
        doc["timestamp"] = millis();
        return publish(statusTopic, doc);
    }
    
    bool publishMotion(uint8_t state, uint16_t distance) {
        StaticJsonDocument<128> doc;
        doc["state"] = state;
        doc["distance"] = distance;
        doc["timestamp"] = millis();
        return publish(motionTopic, doc);
    }
    
    bool publishEnvironment(float temp, float humidity, float pressure) {
        StaticJsonDocument<256> doc;
        doc["temperature"] = temp;
        doc["humidity"] = humidity;
        doc["pressure"] = pressure;
        doc["timestamp"] = millis();
        return publish(environmentTopic, doc);
    }
    
    bool publishLight(float lux) {
        StaticJsonDocument<128> doc;
        doc["lux"] = lux;
        doc["timestamp"] = millis();
        return publish(lightTopic, doc);
    }
    
    bool publishSound(float rms, int peak) {
        StaticJsonDocument<128> doc;
        doc["rms"] = rms;
        doc["peak"] = peak;
        doc["timestamp"] = millis();
        return publish(soundTopic, doc);
    }
    
    bool publishTamper(bool tampered) {
        StaticJsonDocument<128> doc;
        doc["tampered"] = tampered;
        doc["timestamp"] = millis();
        return publish(tamperTopic, doc);
    }
    
    bool publishAlarm(bool triggered, int trigger) {
        StaticJsonDocument<128> doc;
        doc["triggered"] = triggered;
        doc["trigger"] = trigger;
        doc["timestamp"] = millis();
        return publish(alarmTopic, doc);
    }
    
    using CommandCallback = std::function<void(const JsonDocument&)>;
    void setCommandCallback(CommandCallback callback) {
        commandCallback = callback;
    }
    
private:
    void reconnect() {
        static unsigned long lastAttempt = 0;
        unsigned long now = millis();
        
        if (now - lastAttempt < Config::MQTT_RECONNECT_DELAY) {
            return;
        }
        
        lastAttempt = now;
        
        if (client.connect(deviceId.c_str())) {
            publishStatus(true);
            client.subscribe(commandTopic.c_str());
        }
    }
    
    void handleCallback(char* topic, uint8_t* payload, unsigned int length) {
        if (std::string(topic) != commandTopic || !commandCallback) {
            return;
        }
        
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload, length);
        
        if (!error) {
            commandCallback(doc);
        }
    }
    
    bool publish(const std::string& topic, JsonDocument& doc) {
        char buffer[512];
        size_t n = serializeJson(doc, buffer);
        return client.publish(topic.c_str(), buffer, n);
    }
    
    WiFiClient espClient;
    PubSubClient client;
    std::string deviceId;
    
    std::string statusTopic;
    std::string motionTopic;
    std::string environmentTopic;
    std::string lightTopic;
    std::string soundTopic;
    std::string tamperTopic;
    std::string commandTopic;
    std::string responseTopic;
    std::string thresholdsTopic;
    std::string thresholdsSetTopic;
    std::string alarmTopic;
    
    CommandCallback commandCallback;
};
