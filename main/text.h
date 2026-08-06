/*
** wifiManager.cpp
** 05 août 2026
** TRAN Olivier
*/


#ifndef TEXT_H
    #define TEXT_H

#include "HT_SSD1306Wire.h"

extern SSD1306Wire display;

void VextON();
void VextOFF();
void init_text();
void draw_small_text(int x, int y, String text); // note, it can cause lag especially if used in loop();
void draw_big_text(int x, int y, String text);


#endif