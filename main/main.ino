/*
** main.ino
** 03 août 2026
** TRAN Olivier
*/

#include <Wire.h>
#include "HT_SSD1306Wire.h"
#include "wifiManager.h"

WifiManager wifi;

// Display screen stuff, completely optional
SSD1306Wire display(
    0x3c,
    500000,
    SDA_OLED,
    SCL_OLED,
    GEOMETRY_128_64,
    RST_OLED
);

void VextON() // VextON and VextOFF turns ON/OFF the display.
{
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);   // Turn OLED power on
}

void VextOFF()
{
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH);
}

void init_text()
{
    VextON();
    display.init();
    display.clear();

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "Hello World!");
    display.display();
}

int x_offset = -120;
int current_delay = 0;

void scrolling_text(int start_x, int y, int delay_time, int x_inc)
{
    display.clear();
    display.drawString(0 + x_offset, y, "Code is running...");
    display.display();
    if (current_delay >= delay_time) {
        x_offset += x_inc;
        current_delay = 0;
    }
    if (x_offset > 200)
        x_offset = start_x;
    current_delay++;
}

// ################
void setup()
{
    Serial.begin(9600);
    init_text();
    pinMode(LED_BUILTIN, OUTPUT);
    wifi.begin();
    delay(2000);
}

void loop()
{
    wifi.loop();

    if (wifi.isConnected())
    {
        static unsigned long last = 0;

        if (millis() - last > 5000)
        {
            last = millis();

            Serial.println(wifi.getIP());
        }
    }
    scrolling_text(-120, 0, 20, 1);
}
