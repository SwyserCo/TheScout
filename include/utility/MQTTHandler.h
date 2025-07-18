#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

class MQTTHandler {
public:
    MQTTHandler();
    void begin(const String& deviceId);
    void loop();
    bool isConnected() const;
    
    // Publishing methods
    void publishSensorData(const String& sensor, const DynamicJsonDocument& data);
    void publishAlarmState(const String& state);
    void publishDeviceState(const String& state);
    void publishRelayState(bool state);
    
    // Home Assistant auto-discovery
    void publishDiscoveryConfig();
    
    // Subscription callback
    void setAlarmCallback(std::function<void(const String&)> callback);
    void setRelayCallback(std::function<void(bool)> callback);
    void setThresholdCallback(std::function<void(const String&, float)> callback);
    
private:
    WiFiClient _wifiClient;
    PubSubClient _client;
    String _deviceId;
    String _baseTopic;
    uint32_t _lastReconnectAttempt;
    
    std::function<void(const String&)> _alarmCallback;
    std::function<void(bool)> _relayCallback;
    std::function<void(const String&, float)> _thresholdCallback;
    
    bool reconnect();
    void onMessage(char* topic, byte* payload, unsigned int length);
    void publishDiscoveryDevice(const String& component, const String& name, const String& deviceClass = "");
    void publishDiscoveryBinarySensor(const String& name, const String& deviceClass, const String& stateTopic);
    void publishDiscoverySensor(const String& name, const String& deviceClass, const String& stateTopic, const String& unit = "");
    void publishDiscoverySwitch(const String& name, const String& stateTopic, const String& commandTopic);
    void publishDiscoveryAlarm(const String& name, const String& stateTopic, const String& commandTopic);
    
    static void staticMessageCallback(char* topic, byte* payload, unsigned int length);
    static MQTTHandler* instance;
};
