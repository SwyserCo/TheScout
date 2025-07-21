# Product Requirements Document: The Scout - Phase 4

## 1. Phase Goal: Sensor Integration & Data Acquisition

With the device's core functionality established, the goal of this phase is to bring the sensors to life. This involves writing stable, non-blocking code to initialize each sensor and read data from it periodically.

## 2. Core Features & Logic

### User Story
**As a developer**, I want to create a modular and efficient system to manage all of The Scout's sensors, so that I can reliably read data from them without impacting the device's responsiveness.

### Acceptance Criteria
1.  **Sensor Manager**:
    * Create a central `SensorManager` class (`sensors/SensorManager.h` & `.cpp`) responsible for initializing and orchestrating all sensor readings.
    * This class should have a public method, e.g., `update()`, that is called in the main `loop()`. This method will internally manage the polling for each sensor using a non-blocking `millis()` timer.
2.  **Individual Sensor Wrappers**: For each physical sensor, create a dedicated wrapper class that abstracts the library calls. This adheres to the SRP.
    * **Example**: `BME280_Sensor.h` / `.cpp` will wrap the `Adafruit BME280 Library`. It will have methods like `getTemperature()`, `getHumidity()`, etc.
3.  **I2C Sensor Integration**:
    * The I2C bus (SDA: IO17, SCL: IO18) must be initialized.
    * **BME280 (0x76)**: Read temperature, humidity, and pressure. **Polling Frequency**: No more than once every 10 seconds.
    * **VEML7700 (0x10)**: Read ambient light (lux). **Polling Frequency**: No more than once every 5 seconds.
    * **LIS2DW12TR (0x19)**: This sensor is for **tamper detection**. Configure it to generate an interrupt on `IO09` when significant motion is detected. Create a function that sets a boolean flag when this interrupt is triggered.
4.  **UART Sensor Integration**:
    * **LD2420 (RX: IO15, TX: IO16)**: Initialize the mmWave sensor. Read presence detection data (still, moving, or no presence).
5.  **Analog Sensor Integration**:
    * **MEMS Microphone (IO41)**: The goal is to detect loud noises. Read the analog value and if it exceeds a predefined threshold, set a boolean flag.
6.  **GPIO Control**:
    * **Relay (IO12)**: Implement functions within the `SensorManager` or a dedicated `RelayController` class to control the relay (`on()`, `off()`, `toggle()`).
7.  **User Feedback (Using Phase 1 Modules)**:
    * **Activity LED (PIXEL_ACTIVITY)**: Use the `FeedbackManager` to blink the Activity LED with `GUARDIAN_BLUE` briefly every time the `SensorManager` completes a full cycle of sensor reads.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: Follow the Single Responsibility Principle. The `SensorManager` orchestrates, but each individual sensor's logic is contained within its own wrapper class.
2.  **Use Specified Libraries**: You **must** use the libraries specified in the `Master_PRD.md` for each sensor.
3.  **Code Structure**: Create the `SensorManager` and individual sensor wrapper classes inside the `sensors/` directory.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to print sensor readings to the Serial Monitor to verify they are working correctly. The Activity LED should be blinking.