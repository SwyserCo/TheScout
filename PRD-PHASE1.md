# Product Requirements Document: The Scout - Phase 1

## 1. Phase Goal: Implement a Robust WiFi Manager

The primary goal of this phase is to create a user-friendly and reliable WiFi setup mechanism for "The Scout" device. The device must be able to connect to a user's local WiFi network through a captive portal. This phase focuses exclusively on network connectivity and initial device configuration.

---

## 2. Core Feature: Branded Captive Portal for WiFi Setup

### User Story
**As a new user**, I want to set up my Scout device easily by connecting it to my home WiFi network, so I can get it online without needing technical expertise or a computer.

### Acceptance Criteria
1.  **AP Mode Trigger**: On first boot, or if saved WiFi credentials fail to connect, the device must create its own WiFi Access Point (AP).
    * The AP SSID should be `Guardian-Scout-Setup`.
2.  **Automatic Captive Portal**: When a user connects to the `Guardian-Scout-Setup` AP with a phone or laptop, a captive portal should open automatically, directing them to the configuration page.
3.  **Branded Configuration Page**: The web page must be visually appealing and clearly branded.
    * **Header**: "Guardian Security System"
    * **Sub-header**: "The Scout: Network Setup"
    * **Introduction**: A brief (2-3 sentences) explanation, e.g., "Welcome! Let's get your Scout connected. Please select your home WiFi network from the list below and enter the password."
    * The page must automatically scan for and list available WiFi networks.
    * It must provide input fields for the selected SSID and password.
4.  **Credential Handling**:
    * Upon submission, the device will attempt to connect to the selected WiFi network.
    * The WiFi credentials (SSID and password) **must** be securely stored in the ESP32's non-volatile memory using the `Preferences` library.
5.  **Unique Device ID**: After a successful first-time connection, the device must:
    * Generate a unique `DeviceID`.
    * The format must be `TheScout-XXXXXX`, where `XXXXXX` is a unique identifier (e.g., from the last 3 bytes of the MAC address).
    * This `DeviceID` **must** also be saved using the `Preferences` library.
6.  **User Feedback**:
    * **System LED (IO09)**:
        * *Pulsing Blue*: Device is in AP/Setup mode.
        * *Blinking Yellow*: Attempting to connect to the user's WiFi.
        * *Solid Green (for 5 seconds)*: Successfully connected.
    * **Buzzer (IO40)**:
        * Play a short, pleasant "success" chime upon successful WiFi connection.
        * Play a distinct "failure" tone if the connection fails, after which it should revert to AP mode.
7.  **Success & Reboot**: After a successful connection, the device should display a success page ("Setup complete! Your Scout is now connected.") for 10 seconds and then reboot to begin normal operation.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Strategize First**: Before writing code, please outline your plan. For example: "I will create a `WiFiSetupManager` class. This class will handle starting the AP, running the DNS and Web servers, rendering the HTML for the captive portal, and saving the credentials."
2.  **Use Specified Libraries**:
    * `WiFi.h`, `WebServer.h`, `DNSServer.h` for the captive portal functionality.
    * `Preferences.h` for storing all configuration data (WiFi credentials, DeviceID).
3.  **Code Structure**:
    * Create a dedicated class (e.g., `WiFiSetupManager`) to encapsulate all setup logic.
    * The HTML/CSS for the web page should be stored in a separate `.h` file as a `const char[]` PROGMEM variable to keep `main.cpp` clean.
    * The code must be non-blocking. Do not use long `delay()` calls.
4.  **Pin Definitions**: Use the provided pin map from `copilot-background.md` for the System LED and Buzzer.
5.  **Verification**: At the end of this phase, the code must compile successfully using the `pio run` command. The primary test is to confirm the captive portal works as described and the device successfully connects to a network.