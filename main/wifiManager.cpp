/*
** wifiManager.cpp
** 05 août 2026
** TRAN Olivier
*/

#include "WifiManager.h"
#define PRG_BUTTON 0

void WifiManager::begin()
{
    pinMode(PRG_BUTTON, INPUT_PULLUP);
    prefs.begin("wifi", false);

    if (digitalRead(PRG_BUTTON) == LOW) {
        Serial.println("Entering configuration mode...");
        startPortal();
        return;
    }
    connectStored();

    if (WiFi.status() != WL_CONNECTED)
    {
        startPortal();
    }
}

static wl_status_t lastStatus = WL_IDLE_STATUS;

void WifiManager::loop()
{
    wl_status_t current = WiFi.status();

    if (current != lastStatus)
    {
        Serial.printf("WiFi status changed: %d\n", current);
        lastStatus = current;
    }
    if (configMode)
    {
        dns.processNextRequest();
        server.handleClient();
    }
    if (!configMode && current != WL_CONNECTED)
    {
        WiFi.reconnect();
    }
}

bool WifiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

String WifiManager::getIP()
{
    return WiFi.localIP().toString();
}

String WifiManager::getSSID()
{
    return WiFi.SSID();
}

void WifiManager::connectStored()
{
    String ssid = prefs.getString("ssid", "");
    String pass = prefs.getString("pass", "");

    if (ssid == "")
        return;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        if (millis() - start > 15000)
            return;
    }
    Serial.println("Connected.");
}

void WifiManager::startPortal()
{
    configMode = true;

    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32-Setup", "76543210");
    dns.start(53, "*", WiFi.softAPIP());
    setupRoutes();
    server.begin();
    Serial.println("Portal started.");
    Serial.println("Connect to 'ESP32-Setup', password: 76543210");
    Serial.println("Go to: http://192.168.4.1/");
}

void WifiManager::setupRoutes()
{
    server.on("/", HTTP_GET, [this]()
    {
        String html;

        html += "<html><body>";
        html += "<h2>WiFi Setup</h2>";
        html += "<form action='/save' method='POST'>";
        html += "SSID:<br>";
        html += "<input name='ssid'><br>";
        html += "Password:<br>";
        html += "<input name='pass' type='password'><br><br>";
        html += "<input type='submit'>";
        html += "</form></body></html>";
        server.send(200,"text/html",html);
    });

    server.on("/save", HTTP_POST, [this]()
    {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");

        saveCredentials(ssid, pass);
        server.send(200,"text/html","Saved. Rebooting...");
        delay(1500);
        ESP.restart();
    });
    server.on("/forget", HTTP_POST, [this]()
    {
    prefs.clear();
    server.send(200, "text/plain", "Credentials erased. Rebooting...");
    delay(1000);
    ESP.restart();
    });
}

void WifiManager::saveCredentials(String ssid, String password)
{
    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
    WiFi.disconnect(true);
}
