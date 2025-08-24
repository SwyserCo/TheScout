#pragma once

#include <Arduino.h>

class FeedbackManager {
public:
    FeedbackManager(); // Constructor
    void begin();
    void update();
};
