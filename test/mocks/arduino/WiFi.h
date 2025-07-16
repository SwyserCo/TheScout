#pragma once
#include <string>
#include <cstdint>

#ifndef SERIAL_8N1
#define SERIAL_8N1 0x800001c
#endif

class WiFiClass {
public:
    enum class wl_status_t {
        WL_NO_SHIELD = 255,
        WL_IDLE_STATUS = 0,
        WL_NO_SSID_AVAIL = 1,
        WL_SCAN_COMPLETED = 2,
        WL_CONNECTED = 3,
        WL_CONNECT_FAILED = 4,
        WL_CONNECTION_LOST = 5,
        WL_DISCONNECTED = 6
    };

    bool begin(const char* ssid, const char* pass) { return true; }
    wl_status_t status() { return wl_status_t::WL_CONNECTED; }
    bool isConnected() { return true; }
    void disconnect() {}
    std::string macAddress() { return "00:11:22:33:44:55"; }
};

extern WiFiClass WiFi;
