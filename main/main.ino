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
#define PIN_BUTTON 10
#define X_JOYSTICK 11
#define Y_JOYSTICK 12

/* Other macros */
#define STARTUP_TIME 1500

String webSocketServer; // ex "192.168.0.63";
const uint16_t webSocketPort = 8080;
const char* webSocketPath = "/";

bool webSocketConnected = false;
bool lastButtonState = false;

void sendSensorData()
{
    if (!webSocketConnected)
        return;
    JsonDocument doc;

    doc["type"] = "sensor_data";

    // Button
    doc["button"]["pressed"] = (digitalRead(PIN_BUTTON) == LOW);

    /*
     * doc["temperature"] = temperature;
     * doc["gyro"]["x_axis"] = x_axis;
     */

    String json;
    serializeJson(doc, json);

    //Serial.printf("%lu", millis());
    webSocket.sendTXT(json);

    Serial.print("Sent: ");
    Serial.println(json);
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

    initDisplay();

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

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

    if (connected != lastConnected)
    {
        display.clearDisplay();

        // Wi-Fi status
        if (connected)
            display.println("Wi-Fi: Connected");
        else
            display.println("Wi-Fi: Not connected");

        // WebSocket status (broken for the moment)
        /*
        if (webSocketConnected)
            display.println("Server: Connected");
        else
            display.println("Server: Not connected");
        */

        display.display();
        lastConnected = connected;
    }

    if (connected)
    {
        webSocket.loop();
    }

    bool buttonPressed = (digitalRead(PIN_BUTTON) == LOW);
        sendSensorData();
}
