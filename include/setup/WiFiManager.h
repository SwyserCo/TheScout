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
  const char* getHostname();  // New method to retrieve the hostname

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
};
