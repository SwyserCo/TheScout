# Product Requirements Document: The Scout - Phase 2

## 1. Phase Goal: Implement a Robust WiFi Manager

The primary goal of this phase is to create a user-friendly and reliable WiFi setup mechanism for "The Scout" device using the `WiFiManager` library.

## 2. Core Features & Logic

### A. Connection Logic Flow
1.  **Check for Credentials**: On boot, check `Preferences` for stored WiFi credentials.
2.  **Scenario A: Credentials Exist**: Attempt to connect. On failure, begin the "Persistent Retry" strategy. **Do not fall back to AP mode.**
3.  **Scenario B: No Credentials Exist**: Immediately start the Captive Portal / AP Mode.

### B. Persistent Retry Strategy (For Saved Credentials)
1.  **Initial Attempts**: Try to reconnect 5 times (10s delay).
2.  **Long-Term Attempts**: If initial attempts fail, switch to retrying once every 5 minutes.

### C. Captive Portal / AP Mode
1.  **AP SSID**: `TheScout-Setup`
2.  **Webpage UI**: Must be a single-page application that automatically scans for networks and populates a dropdown. Must have a password field with a show/hide toggle.
3.  **Post-Submission Flow**: On failure during this initial setup, the device must restart the AP and show an error on the page. It should not enter the persistent retry loop.

### D. Credential & Device Name Management
1.  **Storage**: All credentials and the device name **must** be stored using the `Preferences` library.
2.  **Device Name Generation**: During the first successful setup, generate and store a unique device name in the format `TheScout-XXXXXX`.

### E. User Feedback (Integration with Phase 1 Modules)
The `FeedbackManager` must be used to provide clear status indications:

| State                       | System LED (PIXEL_SYSTEM) Pattern | Buzzer Action       |
| --------------------------- | --------------------------------- | ------------------- |
| In AP / Captive Portal Mode | Pulsing Blue                      | None                |
| Connecting (with credentials) | Flashing Blue                     | None                |
| Connection Succeeded        | Flash Green twice                 | `playFanfare()`     |
| Connection Failed (with credentials) | Flash Red twice (after 5 tries)   | `playHornBlast()`   |

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `WifiHandler` class must be solely responsible for network connectivity and the captive portal.
2.  **Use Specified Libraries**: You **must** use the `WiFiManager` library (`tzapu/WiFiManager`).
3.  **Code Structure**: Create a `WifiHandler.h` and `WifiHandler.cpp` inside the `network/` directory.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed.