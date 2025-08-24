#include <Arduino.h>
#include "network/WifiHandler.h"
#include "config/Settings.h"

WifiHandler::WifiHandler(FeedbackManager& feedbackManager) 
    : feedbackManager(feedbackManager), currentState(WifiState::DISCONNECTED), 
      lastRetryAttempt(0), retryCount(0), deviceID("") {
}

bool WifiHandler::begin() {
    Serial.println("[WIFI] WifiHandler::begin() called");
    return true;
}

void WifiHandler::update() {
    // Serial.println("[WIFI] WifiHandler::update() called");
}

bool WifiHandler::isConnected() {
    Serial.println("[WIFI] WifiHandler::isConnected() called");
    return false;
}

String WifiHandler::getDeviceID() {
    Serial.println("[WIFI] WifiHandler::getDeviceID() called");
    return deviceID;
}

WifiState WifiHandler::getState() {
    return currentState;
}
