#include "network/WifiHandler.h" // IMPORTANT: Must include its own header

WifiHandler::WifiHandler() {
    // Constructor body can be empty
}

void WifiHandler::begin() {
    #ifdef DEBUG
    Serial.println("WifiHandler::begin() called");
    #endif
}

void WifiHandler::update() {
    // This method is called in the main loop
}
