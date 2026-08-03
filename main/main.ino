#include <Wire.h>
#include "HT_SSD1306Wire.h"

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

void startup()
{
    VextON();
    delay(100);

    display.init();
    display.clear();

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "Hello World!");
    display.display();
}

void setup()
{
    startup();
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}