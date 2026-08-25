#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 8
#define OLED_SCL 9
#define OLED_ADDR 0x3C

extern Adafruit_SSD1306 display;

void initDisplay();

#endif