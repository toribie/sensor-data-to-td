/*
** main.ino
** 03 août 2026
** TRAN Olivier
** Sketch is hitting 88% of program storage space (1160795/1219720)
*/

#include <Wire.h>
#include "wifiManager.h"
#include "text.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

WifiManager wifi;
WebSocketsClient webSocket;

/* Pins */
#define PRG_BUTTON 0

/* Other macros */
#define STARTUP_TIME 1500

String webSocketServer; // ex "192.168.0.63";
const uint16_t webSocketPort = 8080;
const char* webSocketPath = "/";

unsigned long lastTest = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
    switch (type)
    {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket disconnected");
            break;

        case WStype_CONNECTED:
            Serial.println("WebSocket connected");

            // Send an initial test message
            {
                JsonDocument doc;
                doc["type"] = "test";
                doc["message"] = "Hello from ESP32";

                String json;
                serializeJson(doc, json);

                webSocket.sendTXT(json);

                Serial.print("Sent: ");
                Serial.println(json);
            }
            break;

        case WStype_TEXT:
            Serial.print("Received: ");
            Serial.println((char*)payload);
            break;

        default:
            break;
    }
}

void setup()
{
    Serial.begin(9600);
    init_text();
    pinMode(LED_BUILTIN, OUTPUT);
    display.drawString(0, 0, "Hello World!");
    display.display();
    wifi.begin();

    webSocketServer = wifi.getServerIP();
    if (webSocketServer.isEmpty()) {
        webSocketServer = "192.168.0.63";  // fallback ip
    }
    Serial.print("WebSocket server: ");
    Serial.println(webSocketServer);

    delay(1500);
    display.clear();
    display.setFont(ArialMT_Plain_16);
    webSocket.begin(webSocketServer.c_str(), webSocketPort, webSocketPath);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop()
{
    wifi.loop();

    static bool lastConnected = false;
    bool connected = wifi.isConnected();

    if (connected != lastConnected)
    {
        display.clear();

        if (connected)
            display.drawString(0, 0, "Connected");
        else
            display.drawString(0, 0, "Not connected");
        display.display();
        lastConnected = connected;
    }
    if (connected)
    {
        webSocket.loop();
    }
    // Send a test JSON message every 3 minutes
    if (connected && millis() - lastTest >= 120000)
    {
        lastTest = millis();

        JsonDocument doc;
        doc["type"] = "test";
        doc["message"] = "Hello from ESP32";
        doc["millis"] = millis();

        String json;
        serializeJson(doc, json);
        webSocket.sendTXT(json);
        Serial.print("Sent: ");
        Serial.println(json);
    }
}
