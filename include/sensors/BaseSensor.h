#pragma once
#include <functional>

class BaseSensor {
public:
    virtual ~BaseSensor() = default;
    
    // Initialize the sensor
    virtual bool begin() = 0;
    
    // Read sensor data
    virtual bool read() = 0;
    
    // Check if sensor is ready/available
    virtual bool isReady() const = 0;
    
    // Get last error if any
    virtual const char* getLastError() const = 0;
    
    // Set callback for data updates
    using UpdateCallback = std::function<void(void)>;
    void setUpdateCallback(UpdateCallback callback) {
        updateCallback = callback;
    }

protected:
    // Notify listeners of data update
    void notifyUpdate() {
        if (updateCallback) {
            updateCallback();
        }
    }
    
    UpdateCallback updateCallback;
};
