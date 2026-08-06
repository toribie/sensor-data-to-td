/*
** text.cpp
** 06 août 2026
** TRAN Olivier
*/

#include "text.h"

SSD1306Wire display (
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
    display.display();
}

void draw_small_text(int x, int y, String text)
{
  display.setFont(ArialMT_Plain_10);
  display.drawString(x, y, text);
  display.display();
}
