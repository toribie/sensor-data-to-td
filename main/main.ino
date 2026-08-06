/*
** main.ino
** 03 août 2026
** TRAN Olivier
*/

#include <Wire.h>
#include "wifiManager.h"
#include "text.h"

WifiManager wifi;

/* Pins*/
#define PRG_BUTTON 0

/* Other macros*/
#define STARTUP_TIME 1500

// ################
void setup()
{
    Serial.begin(9600);
    init_text();
    pinMode(LED_BUILTIN, OUTPUT);
    display.drawString(0, 0, "Hello World!");
    display.display();
    wifi.begin();
    delay(1500);
    display.clear();
    display.setFont(ArialMT_Plain_16);
}

void loop()
{
    display.clear();
    wifi.loop();

    if (wifi.isConnected())
    {
        display.drawString(0, 0, "Connected");
        static unsigned long last = 0;

        if (millis() - last > 5000) // every 5 seconds print the IP
        {
            last = millis();

            Serial.println(wifi.getIP());
        }
    } else {
        display.drawString(0, 0, "Not connected");
    }
    display.display();
}
