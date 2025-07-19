# Product Requirements Document: The Scout - Phase 2

## 1. Phase Goal: Initialize and Read All Onboard Sensors

With WiFi connectivity established, the goal of this phase is to bring the device's sensors to life. This involves writing stable, non-blocking code to initialize each sensor and read data from it periodically. The data will be stored in memory for now, in preparation for MQTT transmission in the next phase.

---

## 2. Core Features: Sensor Management and Data Acquisition

### User Story
**As a developer**, I want to create a modular and efficient system to manage all of The Scout's sensors, so that I can reliably read data from them without impacting the device's responsiveness.

### Acceptance Criteria
1.  **Sensor Manager**:
    * Create a central `SensorManager` class responsible for initializing and periodically reading data from all sensors.
    * This class should have a public method, e.g., `update()`, that is called in the main `loop()`. This method will internally manage the polling for each sensor using a non-blocking `millis()` timer.
2.  **I2C Sensor Integration**:
    * The I2C bus (SDA: IO17, SCL: IO18) must be initialized.
    * **BME280 (0x76)**: Read temperature, humidity, and pressure. **Polling Frequency**: No more than once every 10 seconds.
    * **VEML7700 (0x10)**: Read ambient light (lux). **Polling Frequency**: No more than once every 5 seconds.
    * **LIS2DW12TR (0x19)**: This sensor is for **tamper detection**, not precise orientation.
        * Configure the sensor to generate an interrupt on the `INT` pin (IO10) when significant motion (a "jolt" or "tap") is detected.
        * Create a function that sets a boolean flag (e.g., `tamperDetected = true;`) when this interrupt is triggered.
3.  **UART Sensor Integration**:
    * **LD2420 (RX: IO15, TX: IO16)**: Initialize the mmWave sensor.
        * Read presence detection data (e.g., still, moving, or no presence).
        * The state should be stored in a simple variable (e.g., an `enum`).
4.  **Analog Sensor Integration**:
    * **MEMS Microphone (IO41)**:
        * The goal is to detect loud noises, not perform complex audio analysis.
        * Implement a function to read the analog value from the microphone.
        * If the reading exceeds a predefined threshold for a short duration, set a boolean flag (e.g., `loudNoiseDetected = true;`).
5.  **GPIO Control**:
    * **Relay (IO12)**: Implement a class or functions to control the relay. It should have simple `on()`, `off()`, and `toggle()` methods.
6.  **User Feedback**:
    * **Activity LED (IO48)**: This LED should provide visual confirmation that sensors are being read.
        * Blink the Activity LED briefly every time the `SensorManager` completes a full cycle of sensor reads.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Strategize First**: Outline your plan. For example: "I will create a `SensorManager` class. Inside it, I will instantiate objects for each sensor. The `update()` method will use `millis()` timers to check if it's time to poll each sensor. I will use the libraries specified in the original PRD."
2.  **Find Better Libraries**: For the specified sensors, please search for the most robust and well-maintained Arduino libraries. If you find a better alternative to the ones in the original PRD, suggest it and explain why it's better.
3.  **Best Practices**:
    * Follow the file structure outlined in `copilot-instructions.md`, creating separate `.h` and `.cpp` files for each sensor driver or logical component.
    * All sensor reading logic **must** be non-blocking.
    * For the accelerometer, research the LIS2DW12's "tap detection" or "activity detection" features to implement the interrupt-driven tamper alert efficiently.
4.  **Verification**: At the end of this phase, the code must compile successfully. Using the Serial Monitor, print the sensor readings as they are updated to verify that all components are working correctly.