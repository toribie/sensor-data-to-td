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
#include "gyro.h"

WifiManager wifi;
WebSocketsClient webSocket;

/* Pins */
#define PRG_BUTTON 0

/* Other macros */
#define STARTUP_TIME 1500
#define SEND_DELAY 50  //in millisecs

String webSocketServer; // ex "192.168.0.63";
const uint16_t webSocketPort = 8080;
const char* webSocketPath = "/";

bool webSocketConnected = false;
unsigned long lastSensorSend = 0;

void sendSensorData()
{
    if (!webSocketConnected)
        return;

    JsonDocument doc;
    GyroData gyroData;

    if (!Gyro::read(gyroData))
    {
        Serial.println("Failed to read gyro");
        return;
    }

    doc["type"] = "sensor_data";

    doc["accel"]["x"] = gyroData.accelX;
    doc["accel"]["y"] = gyroData.accelY;
    doc["accel"]["z"] = gyroData.accelZ;

    doc["rotation"]["x"] = gyroData.gyroX;
    doc["rotation"]["y"] = gyroData.gyroY;
    doc["rotation"]["z"] = gyroData.gyroZ;

    String json;
    serializeJson(doc, json);
    webSocket.sendTXT(json);
    //Serial.print("Sent: ");
    //Serial.println(json);
}


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
            sendSensorData();
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
    pinMode(LED_BUILTIN, OUTPUT);

    initDisplay();
    if (!Gyro::begin())
    {
        Serial.println("Gyro init failed.");
    }
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("hello! :)");
    display.display();
    display.setTextSize(1);

    wifi.begin();

    Serial.print("Self IP: ");
    Serial.println(wifi.getIP());

    webSocketServer = wifi.getServerIP();
    if (webSocketServer.isEmpty())
    {
        webSocketServer = "192.168.0.99";  // fallback ip
    }
    Serial.print("Current Server IP: ");
    Serial.println(webSocketServer);

    delay(STARTUP_TIME);

    display.clearDisplay();
    display.setCursor(0, 0);

    webSocket.begin(
        webSocketServer.c_str(),
        webSocketPort,
        webSocketPath
    );
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop()
{
    wifi.loop();

    static bool lastConnected = false;
    bool connected = wifi.isConnected();

    if (connected != lastConnected) // Screen gets refreshed when wifi status changes
    {
        display.clearDisplay();

        if (connected)
            display.println("Wi-Fi: Connected");
        else
            display.println("Wi-Fi: Not connected");

        display.display();
        lastConnected = connected;
    }

    if (connected)
    {
        webSocket.loop();
    }
    if (webSocketConnected && millis() - lastSensorSend >= SEND_DELAY)
    {
        lastSensorSend = millis();
        sendSensorData();
    }
}
