/*
** main.ino
** 01 août 2026
** TRAN Olivier
** Sketch: 1138023 / 1310720 bytes (86%) of program storage space.
** Global vars: 48996 / 327680 bytes (14%) of dynamic memory.
*/

#include "wifiManager.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "display.h"

WifiManager wifi;
WebSocketsClient webSocket;

/* Pins */
#define PRG_BUTTON 0

/* Other macros */
#define STARTUP_TIME 1500

String webSocketServer; // ex "192.168.0.63";
const uint16_t webSocketPort = 8080;
const char* webSocketPath = "/";

unsigned long lastTest = 0; // used to send a message to the server every minute
bool webSocketConnected = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
    switch (type)
    {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket disconnected");
            webSocketConnected = false;
            break;

        case WStype_CONNECTED:
            Serial.println("WebSocket connected");
            webSocketConnected = true;
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
    Serial.println("Hello World!");
    initDisplay();
    pinMode(LED_BUILTIN, OUTPUT);

    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("hello! c:");
    display.display();
    display.setTextSize(1);

    wifi.begin();
    Serial.print("Self IP: ");
    Serial.println(wifi.getIP());

    webSocketServer = wifi.getServerIP();
    if (webSocketServer.isEmpty()) {
        webSocketServer = "192.168.0.99";  // fallback ip
    }
    Serial.print("Current WebSocket IP: ");
    Serial.println(webSocketServer);

    delay(1500);
    display.clearDisplay();
    display.setCursor(0, 0);

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
        display.clearDisplay();

        // Wifi status
        if (connected)
            display.println("Wi-Fi: Connected");
        else
            display.println("Wi-Fi: Not connected");

        // Websocket status
        if (webSocketConnected)
            display.println("WS: Connected");
        else
            display.println("WS: Not connected");

        display.display();
        lastConnected = connected;
    }

    if (connected)
    {
        webSocket.loop();
    }

    // Send a test JSON message every minute ()
    if (connected && millis() - lastTest >= 60000)
    {
        lastTest = millis();

        JsonDocument doc;
        doc["type"] = "test";
        doc["message"] = "Ping!";
        doc["millis"] = millis();

        String json;
        serializeJson(doc, json);
        webSocket.sendTXT(json);
        Serial.print("Sent: ");
        Serial.println(json);
    }
}