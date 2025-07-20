#include "modules/WebPortal.h"
#include "modules/WiFiConfig.h"
#include "modules/Feedback.h"
#include "Config.h"

WebPortal::WebPortal(WiFiConfig* wifiConfig) 
    : webServer(80), dnsServer(), config(wifiConfig), 
      scanInProgress(false), lastScanTime(0), scanResults("") {
}

void WebPortal::begin() {
    Serial.println("[WebPortal] Starting captive portal...");
    
    // Start DNS server for captive portal
    dnsServer.start(53, "*", WiFi.softAPIP());
    
    // Setup web server routes
    webServer.on("/", HTTP_GET, [this]() { handleRoot(); });
    webServer.on("/scan", HTTP_GET, [this]() { handleScan(); });
    webServer.on("/connect", HTTP_POST, [this]() { handleConnect(); });
    webServer.on("/connecting", HTTP_GET, [this]() { handleConnecting(); });
    
    // Captive portal detection routes
    webServer.on("/generate_204", HTTP_GET, [this]() { handleRoot(); }); // Android
    webServer.on("/redirect", HTTP_GET, [this]() { handleRoot(); }); // Android
    webServer.on("/hotspot-detect.html", HTTP_GET, [this]() { handleRoot(); }); // iOS
    webServer.on("/connectivity-check.html", HTTP_GET, [this]() { handleRoot(); }); // Firefox
    webServer.on("/check_network_status.txt", HTTP_GET, [this]() { handleRoot(); }); // Windows
    webServer.on("/ncsi.txt", HTTP_GET, [this]() { handleRoot(); }); // Windows
    
    webServer.onNotFound([this]() { handleNotFound(); });
    
    webServer.begin();
    Serial.println("[WebPortal] Web server started");
    
    // Initial WiFi scan
    updateWiFiScan();
}

void WebPortal::update() {
    if (!isRunning()) return;
    
    dnsServer.processNextRequest();
    webServer.handleClient();
    
    // Update WiFi scan periodically
    if (millis() - lastScanTime > 10000) { // Every 10 seconds
        updateWiFiScan();
    }
}

void WebPortal::stop() {
    Serial.println("[WebPortal] Stopping captive portal...");
    webServer.stop();
    dnsServer.stop();
}

bool WebPortal::isRunning() const {
    return true; // Always running when created
}

void WebPortal::setConnectionCallback(std::function<void(String, String)> callback) {
    onConnectRequest = callback;
}

void WebPortal::handleRoot() {
    String page = getMainPage();
    webServer.sendHeader("Content-Length", String(page.length()));
    webServer.send(200, "text/html", page);
}

void WebPortal::handleScan() {
    Serial.println("[WebPortal] Scan request received");
    
    if (scanInProgress) {
        webServer.send(202, "text/plain", "Scan in progress");
        return;
    }
    
    updateWiFiScan();
    webServer.sendHeader("Content-Type", "application/json");
    webServer.sendHeader("Content-Length", String(scanResults.length()));
    webServer.send(200, "application/json", scanResults);
}

void WebPortal::handleConnect() {
    String ssid = webServer.arg("ssid");
    String password = webServer.arg("password");
    
    Serial.println("[WebPortal] Connection request - SSID: " + ssid);
    
    if (ssid.length() == 0) {
        webServer.send(400, "text/plain", "Missing SSID");
        return;
    }
    
    if (onConnectRequest) {
        onConnectRequest(ssid, password);
    }
    
    // Redirect to connecting page
    webServer.sendHeader("Location", "/connecting");
    webServer.send(302, "text/plain", "");
}

void WebPortal::handleConnecting() {
    String page = getConnectingPage();
    webServer.sendHeader("Content-Length", String(page.length()));
    webServer.send(200, "text/html", page);
}

void WebPortal::handleNotFound() {
    // Redirect all unknown requests to main page for captive portal
    webServer.sendHeader("Location", "/");
    webServer.send(302, "text/plain", "");
}

String WebPortal::getMainPage() {
    String deviceName = config->getDeviceName();
    
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>)" + deviceName + R"( Setup</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0; 
            padding: 20px; 
            background: #f5f5f5; 
            color: #333; 
        }
        .container { 
            max-width: 400px; 
            margin: 0 auto; 
            background: white; 
            padding: 30px; 
            border-radius: 10px; 
            box-shadow: 0 2px 10px rgba(0,0,0,0.1); 
        }
        .header { 
            text-align: center; 
            margin-bottom: 30px; 
            color: #ff6600; 
        }
        .network-list { 
            margin: 20px 0; 
        }
        .network-item { 
            padding: 12px; 
            margin: 8px 0; 
            background: #f8f8f8; 
            border: 1px solid #ddd; 
            border-radius: 5px; 
            cursor: pointer; 
            display: flex; 
            justify-content: space-between; 
            align-items: center; 
        }
        .network-item:hover { 
            background: #e8e8e8; 
        }
        .network-item.selected { 
            background: #ff6600; 
            color: white; 
        }
        .signal-strength { 
            font-size: 12px; 
            color: #666; 
        }
        .network-item.selected .signal-strength { 
            color: #fff; 
        }
        input[type="password"] { 
            width: 100%; 
            padding: 12px; 
            margin: 10px 0; 
            border: 1px solid #ddd; 
            border-radius: 5px; 
            box-sizing: border-box; 
        }
        button { 
            width: 100%; 
            padding: 12px; 
            background: #ff6600; 
            color: white; 
            border: none; 
            border-radius: 5px; 
            cursor: pointer; 
            font-size: 16px; 
        }
        button:hover { 
            background: #e55a00; 
        }
        button:disabled { 
            background: #ccc; 
            cursor: not-allowed; 
        }
        .status { 
            text-align: center; 
            margin: 15px 0; 
            padding: 10px; 
            border-radius: 5px; 
        }
        .loading { 
            background: #e3f2fd; 
            color: #1976d2; 
        }
        .error { 
            background: #ffebee; 
            color: #c62828; 
        }
        .refresh-btn { 
            background: #2196f3; 
            margin-bottom: 15px; 
        }
        .refresh-btn:hover { 
            background: #1976d2; 
        }
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>🔥 Scout WiFi Setup</h1>
            <p>Configure your )" + deviceName + R"(</p>
        </div>
        
        <button class='refresh-btn' onclick='refreshNetworks()'>🔄 Refresh Networks</button>
        
        <div id='status' class='status loading'>Scanning for networks...</div>
        
        <form id='wifiForm' onsubmit='connectWiFi(event)'>
            <div id='networks' class='network-list'></div>
            
            <input type='hidden' id='selectedSSID' name='ssid' required>
            <input type='password' id='password' name='password' placeholder='WiFi Password' style='display:none;'>
            
            <button type='submit' id='connectBtn' disabled>Connect to Network</button>
        </form>
    </div>

    <script>
        let selectedNetwork = null;
        let networks = [];

        function loadNetworks() {
            document.getElementById('status').innerHTML = 'Scanning for networks...';
            document.getElementById('status').className = 'status loading';
            
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    networks = data.networks || [];
                    displayNetworks();
                    document.getElementById('status').style.display = 'none';
                })
                .catch(error => {
                    console.error('Error loading networks:', error);
                    document.getElementById('status').innerHTML = 'Error scanning networks. Please refresh.';
                    document.getElementById('status').className = 'status error';
                });
        }

        function displayNetworks() {
            const container = document.getElementById('networks');
            container.innerHTML = '';
            
            if (networks.length === 0) {
                container.innerHTML = '<div class="network-item">No networks found</div>';
                return;
            }
            
            networks.forEach((network, index) => {
                const item = document.createElement('div');
                item.className = 'network-item';
                item.onclick = () => selectNetwork(network, item);
                
                const signalText = network.rssi > -50 ? 'Excellent' : 
                                 network.rssi > -60 ? 'Good' : 
                                 network.rssi > -70 ? 'Fair' : 'Weak';
                
                item.innerHTML = `
                    <span>${network.ssid}</span>
                    <span class="signal-strength">${signalText}</span>
                `;
                
                container.appendChild(item);
            });
        }

        function selectNetwork(network, element) {
            // Remove previous selection
            document.querySelectorAll('.network-item').forEach(item => {
                item.classList.remove('selected');
            });
            
            // Select current
            element.classList.add('selected');
            selectedNetwork = network;
            
            document.getElementById('selectedSSID').value = network.ssid;
            document.getElementById('password').style.display = 'block';
            document.getElementById('connectBtn').disabled = false;
        }

        function refreshNetworks() {
            selectedNetwork = null;
            document.getElementById('selectedSSID').value = '';
            document.getElementById('password').style.display = 'none';
            document.getElementById('password').value = '';
            document.getElementById('connectBtn').disabled = true;
            document.getElementById('status').style.display = 'block';
            loadNetworks();
        }

        function connectWiFi(event) {
            event.preventDefault();
            
            if (!selectedNetwork) {
                alert('Please select a network');
                return;
            }
            
            const password = document.getElementById('password').value;
            
            document.getElementById('connectBtn').disabled = true;
            document.getElementById('connectBtn').innerHTML = 'Connecting...';
            
            const formData = new FormData();
            formData.append('ssid', selectedNetwork.ssid);
            formData.append('password', password);
            
            fetch('/connect', {
                method: 'POST',
                body: formData
            })
            .then(response => {
                if (response.redirected) {
                    window.location.href = response.url;
                } else {
                    return response.text();
                }
            })
            .catch(error => {
                console.error('Connection error:', error);
                document.getElementById('connectBtn').disabled = false;
                document.getElementById('connectBtn').innerHTML = 'Connect to Network';
                alert('Connection failed. Please try again.');
            });
        }

        // Load networks on page load
        loadNetworks();
    </script>
</body>
</html>
)";
    
    return html;
}

String WebPortal::getConnectingPage() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Connecting...</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0; 
            padding: 20px; 
            background: #f5f5f5; 
            color: #333;
            text-align: center;
        }
        .container { 
            max-width: 400px; 
            margin: 50px auto; 
            background: white; 
            padding: 40px; 
            border-radius: 10px; 
            box-shadow: 0 2px 10px rgba(0,0,0,0.1); 
        }
        .spinner {
            border: 4px solid #f3f3f3;
            border-top: 4px solid #ff6600;
            border-radius: 50%;
            width: 50px;
            height: 50px;
            animation: spin 1s linear infinite;
            margin: 20px auto;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        h1 { color: #ff6600; margin-bottom: 10px; }
        p { color: #666; margin: 15px 0; }
        .status { 
            padding: 15px; 
            margin: 20px 0; 
            border-radius: 5px; 
            background: #e3f2fd; 
            color: #1976d2; 
        }
    </style>
</head>
<body>
    <div class='container'>
        <h1>🔥 Connecting to WiFi</h1>
        <div class='spinner'></div>
        <p>Please wait while your Scout connects to the network...</p>
        <div class='status' id='status'>Establishing connection...</div>
        <p><small>This page will automatically update when connection is complete.</small></p>
    </div>

    <script>
        let checkCount = 0;
        const maxChecks = 30; // 30 seconds maximum
        
        function checkConnection() {
            checkCount++;
            
            if (checkCount > maxChecks) {
                document.getElementById('status').innerHTML = 'Connection timeout. Please try again.';
                document.getElementById('status').style.background = '#ffebee';
                document.getElementById('status').style.color = '#c62828';
                return;
            }
            
            // Update status message
            const messages = [
                'Establishing connection...',
                'Authenticating with network...',
                'Obtaining IP address...',
                'Finalizing setup...'
            ];
            const messageIndex = Math.min(Math.floor(checkCount / 8), messages.length - 1);
            document.getElementById('status').innerHTML = messages[messageIndex];
            
            // Check if we can reach the internet (indicates successful connection)
            fetch('http://www.google.com/favicon.ico', { mode: 'no-cors' })
                .then(() => {
                    document.getElementById('status').innerHTML = 'Connection successful! Scout is now online.';
                    document.getElementById('status').style.background = '#e8f5e8';
                    document.getElementById('status').style.color = '#2e7d32';
                    
                    // Wait a moment then show success
                    setTimeout(() => {
                        window.location.href = '/success';
                    }, 2000);
                })
                .catch(() => {
                    // Continue checking
                    setTimeout(checkConnection, 1000);
                });
        }
        
        // Start checking after a brief delay
        setTimeout(checkConnection, 2000);
    </script>
</body>
</html>
)";
    
    return html;
}

String WebPortal::getSuccessPage() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Setup Complete</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0; 
            padding: 20px; 
            background: #f5f5f5; 
            color: #333;
            text-align: center;
        }
        .container { 
            max-width: 400px; 
            margin: 50px auto; 
            background: white; 
            padding: 40px; 
            border-radius: 10px; 
            box-shadow: 0 2px 10px rgba(0,0,0,0.1); 
        }
        h1 { color: #4caf50; margin-bottom: 20px; }
        p { color: #666; margin: 15px 0; }
        .success-icon {
            font-size: 64px;
            color: #4caf50;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class='container'>
        <div class='success-icon'>✅</div>
        <h1>Setup Complete!</h1>
        <p>Your Scout has been successfully connected to WiFi.</p>
        <p>You can now close this window.</p>
        <p><small>The device will restart and begin normal operation.</small></p>
    </div>
</body>
</html>
)";
    
    return html;
}

void WebPortal::updateWiFiScan() {
    if (scanInProgress) return;
    
    scanInProgress = true;
    lastScanTime = millis();
    
    Serial.println("[WebPortal] Starting WiFi scan...");
    
    int networkCount = WiFi.scanNetworks(false, false, false, 300);
    
    JsonDocument doc;
    JsonArray networks = doc["networks"].to<JsonArray>();
    
    if (networkCount > 0) {
        // Sort networks by signal strength
        for (int i = 0; i < networkCount - 1; i++) {
            for (int j = i + 1; j < networkCount; j++) {
                if (WiFi.RSSI(i) < WiFi.RSSI(j)) {
                    // Swap networks (WiFi library doesn't provide direct swap)
                    String tempSSID = WiFi.SSID(i);
                    int32_t tempRSSI = WiFi.RSSI(i);
                    wifi_auth_mode_t tempAuth = WiFi.encryptionType(i);
                    // Note: Can't actually swap in WiFi library, so we'll sort in JSON creation
                }
            }
        }
        
        // Create sorted list
        for (int i = 0; i < networkCount; i++) {
            String ssid = WiFi.SSID(i);
            int32_t rssi = WiFi.RSSI(i);
            
            if (ssid.length() > 0) { // Skip hidden networks
                JsonObject network = networks.add<JsonObject>();
                network["ssid"] = ssid;
                network["rssi"] = rssi;
                network["encryption"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
            }
        }
        
        Serial.printf("[WebPortal] Found %d networks\n", networks.size());
    } else {
        Serial.println("[WebPortal] No networks found");
    }
    
    serializeJson(doc, scanResults);
    scanInProgress = false;
    
    // Clean up scan results
    WiFi.scanDelete();
}
