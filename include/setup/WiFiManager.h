#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

class WiFiManager {
public:
  WiFiManager();
  void begin();
  void startCaptivePortal();
  bool loadCredentials();
  void saveCredentials(const String& ssid, const String& password);
  void clearCredentials();
  void connectToWiFi();
  const char* getHostname();  // Retrieves the persistent device name

private:
  Preferences preferences;
  WebServer server;
  DNSServer dnsServer;

  void handleRoot();
  void handleSave();

  static void handleGenerate204();
  static void handleConnectTest();
  static void handleNotFound();

  static WebServer* serverInstance;
  static void setServerInstance(WebServer* instance);
  static WebServer& getServer();

  static String deviceName;  // <-- Add this line to store the persistent device name
};
