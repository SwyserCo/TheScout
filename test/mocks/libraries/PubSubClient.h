#pragma once
#include <cstdint>
#include <functional>
#include <string>

class PubSubClient {
public:
    bool connect(const char* id) { return true; }
    bool connected() { return true; }
    void disconnect() {}
    void setServer(const char* hostname, uint16_t port) {}
    bool publish(const char* topic, const char* payload) { return true; }
    bool publish(const char* topic, const uint8_t* payload, unsigned int length) { return true; }
    bool subscribe(const char* topic) { return true; }
    void setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
        this->callback = callback;
    }
    bool loop() { 
        return true; 
    }
    
private:
    std::function<void(char*, uint8_t*, unsigned int)> callback;
};
