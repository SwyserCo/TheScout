 String html = "<!DOCTYPE html>";
    html += "<html><head><meta charset=\"UTF-8\">";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html += "<title>HearthGuard Security System - The Scout Setup</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; padding: 20px; background-color: #f5f5f5; }";
    html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
    html += "h1 { color: #333; text-align: center; margin-bottom: 10px; }";
    html += "h2 { color: #666; text-align: center; margin-bottom: 20px; }";
    html += ".intro { color: #555; line-height: 1.6; margin-bottom: 30px; text-align: center; }";
    html += "input { width: 100%; padding: 12px; margin: 10px 0; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }";
    html += "button { width: 100%; padding: 12px; background-color: #4CAF50; color: white; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }";
    html += "button:hover { background-color: #45a049; }";
    html += "</style></head><body>";
    html += "<div class=\"container\">";
    html += "<h1>HearthGuard Security System</h1>";
    html += "<h2>The Scout</h2>";
    html += "<div class=\"intro\">";
    html += "Welcome to The Scout setup. This page allows you to configure your device's ";
    html += "Wi-Fi connection. Enter your network credentials below to connect The Scout ";
    html += "to your home network and enable integration with Home Assistant. Once connected, ";
    html += "your device will be ready to monitor your space.";
    html += "</div>";
    html += "<form action=\"/save\" method=\"post\">";
    html += "<input type=\"text\" name=\"ssid\" placeholder=\"Wi-Fi Network Name (SSID)\" required>";
    html += "<input type=\"password\" name=\"password\" placeholder=\"Wi-Fi Password\">";
    html += "<button type=\"submit\">Connect to Wi-Fi</button>";
    html += "</form></div></body></html>";
    return html;


    

String WiFiManager::getSuccessPage() {
    String html = "<!DOCTYPE html>";
    html += "<html><head><meta charset=\"UTF-8\">";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html += "<title>Connection Successful</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; padding: 20px; background-color: #f5f5f5; }";
    html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); text-align: center; }";
    html += "h1 { color: #4CAF50; margin-bottom: 20px; }";
    html += "p { color: #555; line-height: 1.6; }";
    html += "</style></head><body>";
    html += "<div class=\"container\">";
    html += "<h1>✓ Success!</h1>";
    html += "<p>The Scout has been successfully connected to your Wi-Fi network. The device will now restart and be available in Home Assistant.</p>";
    html += "<p><strong>Device ID:</strong> " + _deviceId + "</p>";
    html += "</div></body></html>";
    return html;
}