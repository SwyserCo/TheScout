#include "sensors/MicrophoneSensor.h"

bool MicrophoneSensor::begin() {
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);
    
    samples.reserve(Config::MIC_SAMPLES);
    initialized = true;
    return true;
}

bool MicrophoneSensor::read() {
    if (!initialized) {
        lastError = "Sensor not initialized";
        return false;
    }
    
    samples.clear();
    
    // Collect samples
    for (int i = 0; i < Config::MIC_SAMPLES; i++) {
        samples.push_back(adc1_get_raw(ADC1_CHANNEL_6));
        delayMicroseconds(1000000 / Config::MIC_SAMPLE_RATE);
    }
    
    // Calculate RMS
    float sum = 0;
    for (int sample : samples) {
        sum += sample * sample;
    }
    
    rmsLevel = sqrt(sum / Config::MIC_SAMPLES);
    peakLevel = *std::max_element(samples.begin(), samples.end());
    
    notifyUpdate();
    return true;
}

bool MicrophoneSensor::isReady() const {
    return initialized;
}

const char* MicrophoneSensor::getLastError() const {
    return lastError;
}

float MicrophoneSensor::getRMSLevel() const {
    return rmsLevel;
}

int MicrophoneSensor::getPeakLevel() const {
    return peakLevel;
}

const std::vector<int>& MicrophoneSensor::getSamples() const {
    return samples;
}
