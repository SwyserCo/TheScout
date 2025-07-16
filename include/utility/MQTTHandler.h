#pragma once
#include "../Config.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <string>
#include <functional>
#include <cstdint>

class MQTTHandler {
public:
    using CommandCallback = std::function<void(const JsonDocument&)>;
    using ThresholdsCallback = std::function<void(const JsonDocument&)>;
    
    MQTTHandler();
    void begin(const String& deviceId);
    void update();
    
    // Publishing functions
    bool publishStatus(const char* status);
    bool publishMotion(uint8_t presence, uint16_t distance);
    bool publishEnvironment(float temp, float humidity, float pressure);
    bool publishLight(float lux);
    bool publishSound(float rms, float peak);
    bool publishTamper(bool tampered);
    bool publishAlarm(bool triggered, int reason);
    
    // Command handling
    void setCommandCallback(CommandCallback callback);
    void setThresholdsCallback(ThresholdsCallback callback);

private:
    WiFiClient espClient;
    PubSubClient client;
    std::string deviceId;
    CommandCallback commandCallback;
    ThresholdsCallback thresholdsCallback;
    unsigned long lastReconnectAttempt;
    
    // Topics
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
    
    // Helper functions
    void setupTopics();
    void handleCallback(char* topic, uint8_t* payload, unsigned int length);
    bool publishJson(const char* topic, JsonDocument& doc);
    bool reconnect();
    void publishDiscoveryConfig();
};
