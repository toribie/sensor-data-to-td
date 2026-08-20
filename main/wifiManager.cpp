/*
** wifiManager.cpp
** 05 août 2026
** TRAN Olivier
*/

#include "WifiManager.h"
#include "text.h"
#define PRG_BUTTON 0

void WifiManager::begin()
{
    pinMode(PRG_BUTTON, INPUT_PULLUP);
    prefs.begin("wifi", false);

    unsigned long start = millis();
    while (millis() - start < 3000)
    {
        if (digitalRead(PRG_BUTTON) == LOW)
        {
            Serial.println("Entering configuration mode...");

            draw_small_text(0, 20, "Entering config mode.");
            draw_small_text(0, 30, "Connect to 'ESP32-Setup'");
            draw_small_text(0, 40, "Password: '76543210'");
            draw_small_text(0, 50, "Go to 192.168.4.1");

            delay(5000);

            startPortal();
            return;
        }
    }
    connectStored();
    if (WiFi.status() != WL_CONNECTED)
    {
        draw_small_text(0, 30, "Couldn't connect.");
        draw_small_text(0, 40, "Retrying...");
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
        return;
    }

    static unsigned long lastReconnect = 0;
    if (current != WL_CONNECTED)
    {
        if (millis() - lastReconnect >= 5000)
        {
            lastReconnect = millis();

            Serial.println("Attempting reconnect...");
            WiFi.reconnect();
        }
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

String WifiManager::getServerIP()
{
    return prefs.getString("server_ip", "");
}

void WifiManager::connectStored()
{
    String ssid = prefs.getString("ssid", "");
    String pass = prefs.getString("pass", "");

    if (ssid.isEmpty())
        return;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    draw_small_text(0, 20, "Connecting...");

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);

        if (millis() - start >= 15000)
        {
            Serial.println("Connection timed out.");
            WiFi.disconnect(false, false);   // Cancel the current attempt
            return;
        }
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
        html += "<input name='pass' type='password'><br>";
        html += "Server IP:<br>";
        html += "<input name='ip'><br><br>";
        html += "<input type='submit'>";
        html += "</form></body></html>";
        server.send(200,"text/html",html);
    });

    server.on("/save", HTTP_POST, [this]()
    {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        String server_ip = server.arg("ip");

        saveCredentials(ssid, pass, server_ip);
        server.send(200,"text/html","Saved. Rebooting...");
        delay(1500);
        ESP.restart();
    });
}

void WifiManager::saveCredentials(String ssid, String password, String server_ip)
{
    prefs.putString("ssid", ssid);
    prefs.putString("pass", password);
    prefs.putString("server_ip", server_ip);
    WiFi.disconnect(true);
}
