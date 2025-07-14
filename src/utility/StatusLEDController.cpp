#include "utility/StatusLEDController.h"

StatusLEDController::StatusLEDController(uint8_t systemLEDPin, uint8_t activityLEDPin)
  : systemLED(1, systemLEDPin, NEO_GRB + NEO_KHZ800),
    activityLED(1, activityLEDPin, NEO_GRB + NEO_KHZ800) {}

void StatusLEDController::begin() {
  systemLED.begin();
  activityLED.begin();
  systemLED.show();
  activityLED.show();
}

void StatusLEDController::flashSystemRed(int times) {
  for (int i = 0; i < times; i++) {
    systemLED.setPixelColor(0, systemLED.Color(255, 0, 0));
    systemLED.show();
    delay(300);
    systemLED.clear();
    systemLED.show();
    delay(300);
  }
}

void StatusLEDController::flashSystemGreen(int times) {
  for (int i = 0; i < times; i++) {
    systemLED.setPixelColor(0, systemLED.Color(0, 255, 0));
    systemLED.show();
    delay(300);
    systemLED.clear();
    systemLED.show();
    delay(300);
  }
}

void StatusLEDController::setSystemColor(uint8_t r, uint8_t g, uint8_t b) {
  systemLED.setPixelColor(0, systemLED.Color(r, g, b));
  systemLED.show();
}

void StatusLEDController::clearSystem() {
  systemLED.clear();
  systemLED.show();
}
