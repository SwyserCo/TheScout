#pragma once
#include <string>
#include <functional>
#include <map>

class WebServer {
public:
    WebServer(int port = 80) {}
    void begin() {}
    void handleClient() {}
    void on(const char* uri, std::function<void(void)> handler) {}
    void send(int code, const char* contentType, const char* content) {}
    void sendHeader(const char* name, const char* value) {}
    String arg(const char* name) { return ""; }
    String header(const char* name) { return ""; }
    bool hasArg(const char* name) { return false; }
};
