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

void init_text()
{
    VextON();
    display.init();
    display.clear();

    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "Hello World!");
    display.display();
}

void setup()
{
    init_text();
    pinMode(LED_BUILTIN, OUTPUT);
    delay(2000);
}

void scrolling_text(int x_offset)
{
    display.clear();
    display.drawString(0 + x_offset, 0, "Code is running...");
    display.display();
}

int x_offset = -120;

void loop()
{
  x_offset += 1;
  if (x_offset > 200)
    x_offset = -120;
  scrolling_text(x_offset);
  delay(20);
    }