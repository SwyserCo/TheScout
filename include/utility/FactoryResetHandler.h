#pragma once
#include <Arduino.h>
#include "setup/WiFiManager.h"
#include "utility/StatusLEDController.h"

class FactoryResetHandler {
public:
  FactoryResetHandler(WiFiManager& wifiManager, StatusLEDController& ledController);
  void begin();   // Call this once in setup()
  void update();  // Call this from loop()

private:
  WiFiManager& wifiManager;
  StatusLEDController& ledController;

  static constexpr uint8_t RESET_BUTTON_PIN = 2;

  bool isPressed = false;
  bool resetTriggered = false;
  bool flashState = false;

  unsigned long pressStartTime = 0;
  unsigned long lastFlashTime = 0;
};
