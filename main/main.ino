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
#define PRG_BUTTON 0    //used to enter setup mode at the beginning.

/* Other macros */
#define STARTUP_TIME 1000
#define SEND_DELAY 50  //time between sending packets, in millisecs

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
    //Put your variables where the sensor data is collected here.

    doc["type"] = "sensor_data";
    doc["gyro"]["roll"] = gyroData.roll;
    doc["gyro"]["pitch"] = gyroData.pitch;
    doc["gyro"]["yaw"] = gyroData.yaw;

    // doc["example"] = data;
    //         or
    // doc["example"]["value1"] = data1;
    // doc["example"]["value2"] = data2;

    String json;
    serializeJson(doc, json);
    webSocket.sendTXT(json);
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
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("hello! :)");
    display.setTextSize(1);
    display.display();
    //Wi-Fi setup
    wifi.begin();
    Serial.print("Self IP: ");
    Serial.println(wifi.getIP());
    delay(STARTUP_TIME);

    //Gyro setup
    display.println("Calibrating gyro...");
    display.display();
    if (!Gyro::begin())
    {
        Serial.println("Gyro init failed.");
    }

    //WS setup
    webSocketServer = wifi.getServerIP();
    if (webSocketServer.isEmpty())
    {
        webSocketServer = "192.168.0.99";  // fallback ip
    }
    Serial.print("Current Server IP: ");
    Serial.println(webSocketServer);
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
