# Product Requirements Document: The Scout - Phase 2 (Revision 5)

## 1. Phase Goal: Implement a Robust WiFi Manager

The primary goal of this phase is to create a user-friendly and reliable WiFi setup mechanism for "The Scout" device. The device must be able to connect to a user's local WiFi network through a captive portal if and only if it has no credentials stored. If credentials are stored, it must attempt to connect indefinitely without starting the portal.

## 2. Core Features & Logic

### A. Connection Logic Flow
The device's startup and connection logic must follow this exact sequence:

1.  **Check for Credentials**: On boot, check the `Preferences` library for stored WiFi credentials.
2.  **Scenario A: Credentials Exist**
    * Attempt to connect to the stored WiFi network.
    * **On Success**: Proceed to normal operation.
    * **On Failure**: Begin the "Persistent Retry" strategy (see Section 2B). **Crucially, do not fall back to AP/Captive Portal mode.**
3.  **Scenario B: No Credentials Exist**
    * Immediately start the Captive Portal / AP Mode (see Section 2C).

### B. Persistent Retry Strategy (For Saved Credentials)
This strategy only applies if the device has successfully connected to a network at least once in the past and is now failing to reconnect.

1.  **Initial Attempts**: Try to reconnect 5 times, with a 10-second delay between each attempt.
2.  **Long-Term Attempts**: If the initial 5 attempts fail, switch to a long-delay retry strategy, attempting to reconnect once every 5 minutes.

### C. Captive Portal / AP Mode
This mode should only activate if no credentials are found in `Preferences`.

#### C.1. Webpage User Interface & Functionality Requirements
1.  **Single-Page Application Flow**: The entire WiFi setup process must occur on a **single HTML page**. There must be **no page reloads or redirects** until the user submits the form.
2.  **Branding & Text**:
    * **Main Header**: `The Scout`
    * **Introductory Text**: Replace the sub-header with a short introduction. For example: "Welcome to your new Scout device. To get started, please select your home WiFi network from the list below and enter the password to bring it online."
    * **Connect Button Text**: `Connect to Network`
3.  **Loading Overlay & Automatic Scan**:
    * **Appearance**: On page load, a full-screen, semi-transparent overlay (e.g., `rgba(0, 0, 0, 0.7)`) must immediately appear.
    * **Content**: Centered within the overlay, there must be an animated, spinning loader icon (themed with an orange color) and the text "Scanning for networks...<br>Please wait...".
    * **Behavior**: This overlay is displayed while the initial, automatic network scan is in progress. JavaScript will automatically call an API endpoint on the ESP32 (e.g., `/api/scan`) to fetch the list of networks. Once the call completes (successfully or with an error), the overlay must smoothly fade out, revealing the form.
4.  **WiFi Network Selection**:
    * **Clean SSID List**: The `/api/scan` endpoint must return a clean, de-duplicated JSON array of SSID strings. The JavaScript will parse this array and dynamically populate the dropdown menu.
5.  **Password Input**:
    * The password field must include a button or icon that, when pressed, toggles the input field's type between `password` and `text` to allow the user to check their entry.
6.  **Button State**:
    * The "Connect to Network" button must be disabled by default.
    * It should only become enabled after the user has selected a network from the dropdown list.

#### C.2. Visual Target
The final rendered webpage should look exactly like this screenshot.

![Captive Portal UI Target](images/captive_portal_target.png)
*(Note: You will need to create this screenshot and place it in an `images` folder in your project for this link to work.)*

#### C.3. Webpage Post-Submission Flow
1.  After the user clicks "Connect to Network," the device should serve a new page with a "Connecting... Please wait." message.
2.  **On First-Time Success**: The device saves the credentials and reboots into normal operation.
3.  **On First-Time Failure**: If the connection fails during this initial setup, the device must **not** enter the persistent retry loop. Instead, it must immediately restart the AP and return the user to the initial configuration page with an error message (e.g., "Connection failed. Please check the password and try again.").

### D. Credential & Device Name Management
1.  **Storage**: All credentials and the device name **must** be stored using the `Preferences` library.
2.  **Device Name Generation**: During the very first successful setup via the captive portal, the device must:
    * Check if a device name already exists in `Preferences`.
    * If not, generate a unique device name in the format `TheScout-XXXXXX`, where `XXXXXX` is a random 6-digit hexadecimal string.
    * Store this name in `Preferences`.

### E. User Feedback (Integration with Phase 1 Modules)
The `FeedbackManager` must be used to provide clear status indications:

| State                               | System LED (IO09) Pattern         | Buzzer Action                       |
| ----------------------------------- | --------------------------------- | ----------------------------------- |
| In AP / Captive Portal Mode         | Pulsing Blue                      | None                                |
| Connecting (with saved credentials) | Flashing Blue                     | None                                |
| Connection Succeeded                | Flash Green twice                 | `playSuccessChime()`                |
| Connection Failed (with credentials)| Flash Red twice (at end of 5 tries) | `playFailureTone()` (at end of 5 tries) |

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The `WifiHandler` class created in this phase must be solely responsible for network connectivity and the captive portal, as per the Single Responsibility Principle.
2.  **Use Specified Libraries**: You **must** use the `WiFiManager` library (`tzapu/WiFiManager`) to handle the captive portal logic.
3.  **Code Structure**: Create a `WifiHandler.h` and `WifiHandler.cpp` inside the `network/` directory.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to confirm all UI elements and logic flows work exactly as described.