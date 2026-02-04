#ifndef MENUUI_H
#define MENUUI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "GlobalTypes.h"

class MenuUI {
public:
    MenuUI(U8G2* canvas) : u8g2(canvas) {}

    void render(int selectedIndex, DashboardData data) {
        // Header with 8px margin
        u8g2->setFont(u8g2_font_helvB08_tr);
        u8g2->drawRBox(8, 0, 112, 13, 2);
        u8g2->setDrawColor(0);
        u8g2->setCursor(14, 10);
        u8g2->print("HUD CONFIGURATION");
        u8g2->setDrawColor(1);

        const char* items[] = {
            "DISPLAY STYLE",
            "BRIGHTNESS",
            "TRIP RESTART",
            "SAVE & EXIT"
        };

        for(int i=0; i<4; i++) {
            int y = 28 + (i * 12);
            // Selection box shifted inward (8px margin)
            if (i == selectedIndex) {
                u8g2->drawRBox(8, y-10, 112, 12, 2);
                u8g2->setDrawColor(0);
            } else {
                u8g2->setDrawColor(1);
            }
            
            u8g2->setCursor(12, y);
            u8g2->print(items[i]);
            
            // Values aligned to right (100px)
            if (i == 0) {
                u8g2->setCursor(102, y);
                u8g2->printf("%d", data.clockStyle + 1);
            } else if (i == 1) {
                u8g2->setCursor(95, y);
                u8g2->printf("%d%%", (data.brightness * 100) / 255);
            }
            
            u8g2->setDrawColor(1);
        }

        // Bottom border hint
        u8g2->drawHLine(32, 63, 64);
    }

private:
    U8G2* u8g2;
};

#endif
