# HearthGuard Scout Firmware Architecture

## 1. High-Level Overview

This document outlines the software architecture for the HearthGuard Scout firmware. The primary goal of this architecture is to create a robust, maintainable, and extensible system by enforcing a clear separation of concerns. The firmware operates as a non-blocking, state-driven application that reads sensor data, manages network connectivity, and communicates with a central server via MQTT. This document is the "map" that shows how all the individual modules, defined in the phased PRDs, fit together.

---

## 2. Core Architectural Patterns

The entire firmware is built upon these foundational patterns, which are defined in detail in the `copilot-instructions.md` file:

* **Finite State Machine (FSM):** The device's overall behavior is controlled by a main state machine in `main.cpp`. This ensures predictable and sequential operation.
* **Single Responsibility Principle (SRP):** Every class has one, and only one, responsibility.
* **Hardware Abstraction (Wrapper Classes):** All direct interaction with hardware and third-party libraries is done through custom wrapper classes. This decouples the core logic from specific hardware implementations.

---

## 3. System Block Diagram

The firmware is composed of several high-level manager modules that are orchestrated by `main.cpp`.

* **`main.cpp` (Global Objects, State Machine, `setup()`, `loop()`)**
    * Calls `update()` on each of the following manager objects during every loop:
        * **`FeedbackManager`**
            * Manages `LedController`
            * Manages `BuzzerController`
            * Uses the `Preferences` Library for settings
        * **`WifiHandler`**
            * Wraps the `WiFiManager` Library
        * **`DeviceManager`**
            * Manages Factory Reset logic
        * **`SensorManager`**
            * Manages `PirSensor`
            * Manages `Ld2410sSensor`
            * Manages `PowerStatus`
        * **`MqttHandler`**
            * Wraps the `PubSubClient` Library

---

## 4. Module Responsibilities & Public APIs (The Contracts)

This section defines the single responsibility and the public-facing "contract" (key public methods) for each major module.

### A. `FeedbackManager`
* **Responsibility:** To be the single, unified interface for all user-facing feedback (visual and audible).
* **Dependencies:** `LedController`, `BuzzerController`, `Preferences` library.
* **Public API:**
  ```cpp
  // Manages all user feedback.
  class FeedbackManager {
  public:
    void begin(); // Initializes controllers, loads settings from Preferences.
    void update(); // Updates all non-blocking animations and sound sequences.
  
    // --- Global Controls ---
    void setBrightness(uint8_t brightness); // Sets LED brightness and saves to Preferences.
    void setStealthMode(bool enabled); // Enables/disables all feedback and saves to Preferences.
  
    // --- LED Control Delegation ---
    void startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation, uint16_t interval);
    void turnOffLeds();
  
    // --- Buzzer Control Delegation ---
    void playSuccess();
    void playFailure();
    void playConfirm();
    void playInteraction();
  };
  ```

### B. `WifiHandler`
* **Responsibility:** To manage the device's connection to the WiFi network. This includes the initial setup via captive portal and all subsequent reconnection logic.
* **Dependencies:** `FeedbackManager`, `Preferences` library, `WiFiManager` library.
* **Public API:**
  ```cpp
  // Manages WiFi connectivity and setup portal.
  class WifiHandler {
  public:
    void begin(); // Starts the connection or setup process based on saved credentials.
    void update(); // Manages the non-blocking connection logic.
  
    bool isConnected(); // Returns the current WiFi connection status.
    String getDeviceID(); // Returns the unique device ID.
  };
  ```

### C. `DeviceManager`
* **Responsibility:** To manage device-level operations, primarily the factory reset process.
* **Dependencies:** `FeedbackManager`, `Preferences` library.
* **Public API:**
  ```cpp
  // Manages device-level operations like factory reset.
  class DeviceManager {
  public:
    void begin(); // Sets up the factory reset button pin.
    void update(); // Checks for the factory reset button hold in a non-blocking way.
  };
  ```

### D. `SensorManager`
* **Responsibility:** To initialize, read, and process data from all onboard sensors, applying sensor fusion logic to provide a single, clean data object.
* **Dependencies:** `FeedbackManager`, `PirSensor`, `Ld2410sSensor`, `PowerStatus`.
* **Public API:**
  ```cpp
  // Manages all onboard sensors.
  class SensorManager {
  public:
    void begin(); // Initializes all sensor wrappers.
    void update(); // Polls all sensors at their defined intervals.
  
    SensorData getSensorData(); // Returns the latest, fully processed SensorData struct.
  };
  ```

### E. `MqttHandler`
* **Responsibility:** To manage all communication with the MQTT broker. This includes connecting, publishing data, and subscribing to command topics.
* **Dependencies:** `WifiHandler`, `FeedbackManager`, `PubSubClient`, `ArduinoJson`.
* **Public API:**
  ```cpp
  // Manages all MQTT communication.
  class MqttHandler {
  public:
    void begin(); // Sets up the MQTT client with server details.
    void update(); // Maintains the MQTT connection and processes incoming messages.
  
    bool isConnected(); // Returns the current MQTT connection status.
    void publishData(const SensorData& data); // Takes the SensorData struct and publishes it.
  };
  ```

## 5. State Machine Definition

The main `SystemState` enum in `main.cpp` governs the device's high-level behavior. The `loop()` function will contain a `switch` statement that executes the "Ongoing Actions" for the current state.

| State | Purpose | Entry Actions | Ongoing Actions (in `loop()`) | Exit Conditions |
|---|---|---|---|---|
| **`BOOTING`** | Initialize the system. | `Serial.begin()`, call `begin()` on all managers. | None. | Immediately transitions to `CHECK_WIFI_CREDENTIALS`. |
| **`CHECK_WIFI_CREDENTIALS`** | Determine if the device has been configured before. | None. | Check `Preferences` for a saved SSID. | If SSID exists, transition to `CONNECTING_WIFI`. If not, transition to `WIFI_SETUP`. |
| **`WIFI_SETUP`** | Host a captive portal for the user to enter WiFi credentials. | Start the `WifiHandler`'s captive portal. Start the "AP Mode" LED pattern. | The `WifiHandler`'s `update()` method manages the web server. | When the user successfully submits credentials, transition to `CONNECTING_WIFI`. |
| **`CONNECTING_WIFI`** | Actively try to connect to a saved WiFi network. | Start the "Connecting to WiFi" LED pattern. | The `WifiHandler`'s `update()` method manages the connection attempts and retry logic. | If connection succeeds, transition to `CONNECTING_MQTT`. If it fails permanently, transition to `ERROR_WIFI`. |
| **`CONNECTING_MQTT`** | Actively try to connect to the MQTT broker. | Start the "Connecting to MQTT" LED pattern. | The `MqttHandler`'s `update()` method manages the connection attempts and retry logic. | If connection succeeds, transition to `NORMAL_OPERATION`. If it fails after several retries, transition to `ERROR_MQTT`. |
| **`NORMAL_OPERATION`** | The main operational state. The device is fully online. | None. | `SensorManager` reads data. `MqttHandler` publishes data and checks for commands. | If WiFi or MQTT connection is lost, transition back to `CONNECTING_WIFI` or `CONNECTING_MQTT`. |
| **`ERROR_WIFI`** | A fatal state indicating the device cannot connect to WiFi with saved credentials. | Start a solid red "Error" LED pattern. | None. The device is halted. | A factory reset or power cycle is required. |
| **`ERROR_MQTT`** | A state indicating the device is on WiFi but cannot reach the MQTT server. | Start a pulsing orange "Warning" LED pattern. | The `MqttHandler` continues to try reconnecting in the background. | If MQTT connection is restored, transition to `NORMAL_OPERATION`. |

## 6. Data & Control Flow

### A. Control Flow (The "Brain")

1. **`main.cpp` - `setup()`**: Initializes all manager objects by calling their `begin()` methods. Sets the initial `SystemState` to `BOOTING`.
2. **`main.cpp` - `loop()`**:
   * Calls the `update()` method on every manager object on every loop.
   * Executes a `switch` statement based on the current `SystemState` to handle high-level logic and state transitions as defined in the table above.

### B. Data Flow (Sensor to Cloud)

1. **Hardware -> Wrapper Class:** A sensor (e.g., PIR) detects an event. Its wrapper class reads this raw data.
2. **Wrapper Class -> SensorManager:** The `SensorManager` polls the wrapper class, gets the data, and applies fusion logic.
3. **SensorManager -> `main.cpp`:** The `main.cpp` loop, when in the `NORMAL_OPERATION` state, calls `sensorManager.getSensorData()` to get the complete `SensorData` struct.
4. **`main.cpp` -> MqttHandler:** The `main.cpp` loop passes the `SensorData` struct to `mqttHandler.publishData()`.
5. **MqttHandler -> Home Assistant:** The `MqttHandler` formats the data into JSON and publishes it to the MQTT broker.