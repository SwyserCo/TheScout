#include "setup/WiFiManager.h"
#include "utility/StatusLEDController.h"
#include "Preferences.h"
#include "utility/BuzzerController.h"

WebServer* WiFiManager::serverInstance = nullptr;
String WiFiManager::deviceName;
extern BuzzerController buzzer;

#define SYSTEM_LED_PIN 3
#define ACTIVITY_LED_PIN 45

StatusLEDController statusLED(SYSTEM_LED_PIN, ACTIVITY_LED_PIN);

WiFiManager::WiFiManager() : server(80) {}

void WiFiManager::begin() {
  setServerInstance(&server);
  statusLED.begin();

  // Load or generate persistent device name
  Preferences preferences;
  preferences.begin("guardian", false);
  if (!preferences.isKey("deviceName")) {
    String uniqueName = "TheScout-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    preferences.putString("deviceName", uniqueName);
    deviceName = uniqueName;
  } else {
    deviceName = preferences.getString("deviceName");
  }
  preferences.end();

  if (!loadCredentials()) {
    statusLED.flashSystemRed(3);
    startCaptivePortal();
  } else {
    connectToWiFi();
  }
}

bool WiFiManager::loadCredentials() {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();
  return ssid.length() > 0 && password.length() > 0;
}

void WiFiManager::saveCredentials(const String& ssid, const String& password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

void WiFiManager::clearCredentials() {
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();

  preferences.begin("guardian", false);
  preferences.clear();
  preferences.end();
}

void WiFiManager::connectToWiFi() {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();

  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to WiFi");
  

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    statusLED.flashSystemRed(1);
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");
    Serial.println(WiFi.localIP());
    buzzer.beep(2500, 100, 1);

    WiFi.setHostname(deviceName.c_str());
    Serial.print("Hostname set to: ");
    Serial.println(deviceName);

    statusLED.flashSystemGreen(3);
  } else {
    Serial.println("Failed to connect. Starting AP mode.");
    statusLED.flashSystemRed(3);
    startCaptivePortal();
  }
}

void WiFiManager::startCaptivePortal() {
  String apName = "TheScout-Setup";
  WiFi.softAP(apName.c_str());
  Serial.print("Access Point: ");
  Serial.println(apName);

  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", std::bind(&WiFiManager::handleRoot, this));
  server.on("/save", HTTP_POST, std::bind(&WiFiManager::handleSave, this));
  server.on("/generate_204", std::bind(&WiFiManager::handleRoot, this));
  server.on("/connecttest.txt", std::bind(&WiFiManager::handleRoot, this));
  server.onNotFound(std::bind(&WiFiManager::handleRoot, this));

  server.begin();

  Serial.println("Captive portal started. Connect to AP and configure WiFi.");

  unsigned long startTime = millis();
  while (millis() - startTime < 300000) {
    dnsServer.processNextRequest();
    server.handleClient();
  }

  Serial.println("Captive portal timeout. Restarting...");
  ESP.restart();
}

void WiFiManager::handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          padding: 2em;
          background-color: #f4f4f4;
        }
        form {
          background: white;
          padding: 2em;
          border-radius: 8px;
          box-shadow: 0 0 10px rgba(0,0,0,0.1);
          max-width: 300px;
          margin: auto;
        }
        input[type="text"], input[type="password"] {
          width: 100%;
          padding: 0.5em;
          margin: 0.5em 0;
          border: 1px solid #ccc;
          border-radius: 4px;
        }
        input[type="submit"] {
          background-color: #4CAF50;
          color: white;
          padding: 0.7em;
          border: none;
          border-radius: 4px;
          cursor: pointer;
          width: 100%;
        }
        input[type="submit"]:hover {
          background-color: #45a049;
        }
      </style>
    </head>
    <body>
      <form action="/save" method="POST">
        <h2>Connect TheScout to WiFi</h2>
        <input name="ssid" type="text" placeholder="WiFi SSID" required><br>
        <input name="password" type="password" placeholder="WiFi Password" required><br>
        <input type="submit" value="Connect">
      </form>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void WiFiManager::handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    saveCredentials(server.arg("ssid"), server.arg("password"));
    server.send(200, "text/html", "Saved. Rebooting...");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "Missing SSID or password.");
  }
}

void WiFiManager::setServerInstance(WebServer* instance) {
  serverInstance = instance;
}

WebServer& WiFiManager::getServer() {
  return *serverInstance;
}

const char* WiFiManager::getHostname() {
  return deviceName.c_str();
}
