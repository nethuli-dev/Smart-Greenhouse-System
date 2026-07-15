/*
 * ESP8266 Web Dashboard for Greenhouse System
 * Receives data from Arduino and serves web dashboard
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

ESP8266WebServer server(WEB_SERVER_PORT);

// Variables for greenhouse data
float temperature = 0;
float humidity = 0;
int soilMoisture = 0;
bool pumpState = false;
bool fanState = false;
unsigned long lastUpdate = 0;

// HTML content from SPIFFS
void handleRoot()
{
    String html = "<!DOCTYPE html><html>";
    html += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Smart Greenhouse Dashboard</title>";
    html += "<link rel='stylesheet' href='/style.css'>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<h1>🌱 Smart Greenhouse Control</h1>";
    html += "<div class='dashboard'>";

    // Sensor readings
    html += "<div class='card'><h2>📊 Sensor Data</h2>";
    html += "<div class='reading'>🌡️ Temperature: <span id='temp'>" + String(temperature) + "</span>°C</div>";
    html += "<div class='reading'>💧 Humidity: <span id='humidity'>" + String(humidity) + "</span>%</div>";
    html += "<div class='reading'>🌍 Soil Moisture: <span id='soil'>" + String(soilMoisture) + "</span>%</div>";
    html += "<div class='reading'>💡 Pump: <span id='pumpState'>" + String(pumpState ? "ON" : "OFF") + "</span></div>";
    html += "<div class='reading'>🌀 Fan: <span id='fanState'>" + String(fanState ? "ON" : "OFF") + "</span></div>";
    html += "</div>";

    // Control buttons
    html += "<div class='card'><h2>🎮 Manual Controls</h2>";
    html += "<button onclick='controlPump(true)' class='btn on'>💧 Pump ON</button>";
    html += "<button onclick='controlPump(false)' class='btn off'>💧 Pump OFF</button><br><br>";
    html += "<button onclick='controlFan(true)' class='btn on'>🌀 Fan ON</button>";
    html += "<button onclick='controlFan(false)' class='btn off'>🌀 Fan OFF</button>";
    html += "</div>";

    // System status
    html += "<div class='card'><h2>🔄 System Status</h2>";
    html += "<div>Last Update: <span id='lastUpdate'>" + String(millis() / 1000) + "s</span></div>";
    html += "</div>";

    html += "</div></div>";
    html += "<script src='/script.js'></script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleCSS()
{
    String css = R"rawliteral(
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { font-family: 'Segoe UI', Arial, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
           min-height: 100vh; padding: 20px; }
    .container { max-width: 800px; margin: 0 auto; }
    h1 { color: white; text-align: center; margin-bottom: 30px; font-size: 2.5em; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
    .dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
    .card { background: white; border-radius: 15px; padding: 25px; box-shadow: 0 10px 30px rgba(0,0,0,0.2); 
           transition: transform 0.3s ease; }
    .card:hover { transform: translateY(-5px); }
    .card h2 { color: #333; margin-bottom: 15px; border-bottom: 2px solid #667eea; padding-bottom: 10px; }
    .reading { padding: 10px; margin: 8px 0; background: #f8f9fa; border-radius: 8px; display: flex; 
               justify-content: space-between; font-size: 1.1em; }
    .btn { padding: 12px 25px; border: none; border-radius: 8px; font-size: 1em; font-weight: bold; 
           margin: 5px 10px 5px 0; cursor: pointer; transition: all 0.3s ease; }
    .btn.on { background: #4CAF50; color: white; }
    .btn.off { background: #f44336; color: white; }
    .btn:hover { transform: scale(1.05); box-shadow: 0 5px 15px rgba(0,0,0,0.2); }
    @media (max-width: 600px) { .dashboard { grid-template-columns: 1fr; } }
  )rawliteral";
    server.send(200, "text/css", css);
}

void handleJS()
{
    String js = R"rawliteral(
    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').textContent = data.temperature;
          document.getElementById('humidity').textContent = data.humidity;
          document.getElementById('soil').textContent = data.soilMoisture;
          document.getElementById('pumpState').textContent = data.pump ? 'ON' : 'OFF';
          document.getElementById('fanState').textContent = data.fan ? 'ON' : 'OFF';
          document.getElementById('lastUpdate').textContent = Math.floor(Date.now()/1000) + 's';
        })
        .catch(err => console.error('Error updating data:', err));
    }

    function controlPump(state) {
      const cmd = state ? 'PUMP_ON' : 'PUMP_OFF';
      fetch('/control?cmd=' + cmd)
        .then(response => response.text())
        .then(data => {
          console.log('Command sent:', cmd);
          setTimeout(updateData, 500);
        })
        .catch(err => console.error('Error:', err));
    }

    function controlFan(state) {
      const cmd = state ? 'FAN_ON' : 'FAN_OFF';
      fetch('/control?cmd=' + cmd)
        .then(response => response.text())
        .then(data => {
          console.log('Command sent:', cmd);
          setTimeout(updateData, 500);
        })
        .catch(err => console.error('Error:', err));
    }

    // Update data every 2 seconds
    setInterval(updateData, 2000);
    updateData();
  )rawliteral";
    server.send(200, "application/javascript", js);
}

void handleData()
{
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["soilMoisture"] = soilMoisture;
    doc["pump"] = pumpState;
    doc["fan"] = fanState;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleControl()
{
    if (server.hasArg("cmd"))
    {
        String cmd = server.arg("cmd");
        Serial.println("Control command: " + cmd);
        // Send command to Arduino
        Serial2.println(cmd);
        server.send(200, "text/plain", "OK");
    }
    else
    {
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.println("IP address: " + WiFi.localIP().toString());

    server.on("/", handleRoot);
    server.on("/style.css", handleCSS);
    server.on("/script.js", handleJS);
    server.on("/data", handleData);
    server.on("/control", handleControl);

    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();

    // Read data from Arduino
    if (Serial2.available())
    {
        String data = Serial2.readStringUntil('\n');
        data.trim();

        // Parse data string: T:25.5,H:65.2,S:45,P:1,F:0
        int tIdx = data.indexOf("T:");
        int hIdx = data.indexOf(",H:");
        int sIdx = data.indexOf(",S:");
        int pIdx = data.indexOf(",P:");
        int fIdx = data.indexOf(",F:");

        if (tIdx >= 0 && hIdx >= 0 && sIdx >= 0 && pIdx >= 0 && fIdx >= 0)
        {
            temperature = data.substring(tIdx + 2, hIdx).toFloat();
            humidity = data.substring(hIdx + 3, sIdx).toFloat();
            soilMoisture = data.substring(sIdx + 3, pIdx).toInt();
            pumpState = data.substring(pIdx + 3, fIdx).toInt() == 1;
            fanState = data.substring(fIdx + 3).toInt() == 1;

            Serial.println("Updated: T=" + String(temperature) + ", H=" + String(humidity) +
                           ", S=" + String(soilMoisture) + ", P=" + String(pumpState) +
                           ", F=" + String(fanState));
        }
    }
}