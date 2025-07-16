#pragma once

class DNSServer {
public:
    void start(uint16_t port, const char* domainName, const uint8_t* resolvedIP) {}
    void processNextRequest() {}
    void stop() {}
};
