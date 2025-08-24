#include "feedback/FeedbackManager.h" // IMPORTANT: Must include its own header

FeedbackManager::FeedbackManager() {
    // Constructor body can be empty
}

void FeedbackManager::begin() {
    #ifdef DEBUG
    Serial.println("FeedbackManager::begin() called");
    #endif
}

void FeedbackManager::update() {
    // This method is called in the main loop
}
