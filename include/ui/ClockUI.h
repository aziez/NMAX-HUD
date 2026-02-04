#ifndef CLOCKUI_H
#define CLOCKUI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "GlobalTypes.h"
#include "TripManager.h"

class ClockUI {
public:
    ClockUI(U8G2* canvas) : u8g2(canvas) {}

    void render(DashboardData data, int style, bool blink) {
        u8g2->setFontMode(1);
        u8g2->setDrawColor(1);

        switch(style) {
            case 0: renderFuturistic(data); break;
            case 1: renderCockpit(data, blink); break;
            case 2: renderCyberHub(data, blink); break;
            case 3: renderMinimalNeon(data); break;
        }
    }

private:
    U8G2* u8g2;

    void renderFuturistic(DashboardData data) {
        // Clock with techy design - 8px margin
        u8g2->setFont(u8g2_font_logisoso32_tn);
        int w = u8g2->getStrWidth(data.currentTime.c_str());
        u8g2->setCursor(64 - w/2, 40);
        u8g2->print(data.currentTime);
        
        u8g2->drawLine(15, 45, 113, 45);

        u8g2->setFont(u8g2_font_profont12_tf);
        u8g2->setCursor(10, 60);
        u8g2->print("RIDE TIME:");
        u8g2->setCursor(75, 60);
        u8g2->print(TripManager::getFormattedTime(data.tripSeconds));

        u8g2->setCursor(10, 12);
        u8g2->print(data.customName);
        
        u8g2->setCursor(100, 12);
        u8g2->printf("S:%d", data.satellites);
    }

    void renderCockpit(DashboardData data, bool blink) {
        // HUD Style - Inset 2px
        u8g2->drawRFrame(2, 2, 124, 60, 4);
        u8g2->drawLine(2, 18, 126, 18);
        
        u8g2->setFont(u8g2_font_helvB08_tr);
        u8g2->setCursor(10, 14);
        u8g2->print("SPD");
        u8g2->setCursor(95, 14);
        u8g2->print("UTC");

        u8g2->setFont(u8g2_font_logisoso24_tn);
        u8g2->setCursor(10, 46);
        u8g2->printf("%02d", data.currentSpeed);
        
        u8g2->setCursor(68, 46);
        u8g2->print(data.currentTime);

        u8g2->setFont(u8g2_font_profont10_tf);
        u8g2->setCursor(10, 58);
        u8g2->printf("DIST: %.1f KM", data.tripDistance/1000.0);
    }

    void renderCyberHub(DashboardData data, bool blink) {
        // High Information Layout - More space
        u8g2->setFont(u8g2_font_inb16_mn);
        u8g2->setCursor(40, 28);
        u8g2->print(data.currentTime);

        u8g2->setFont(u8g2_font_profont12_tf);
        u8g2->setCursor(10, 48);
        u8g2->printf("SPD: %d", data.currentSpeed);
        u8g2->setCursor(10, 60);
        u8g2->printf("TOP: %d", data.maxSpeed);

        // Right column
        u8g2->setCursor(80, 48);
        u8g2->printf("SAT %d", data.satellites);
        
        u8g2->setCursor(80, 60);
        u8g2->printf("BAT %d%%", data.batteryPhone >= 0 ? data.batteryPhone : 0);
    }

    void renderMinimalNeon(DashboardData data) {
        u8g2->setFont(u8g2_font_logisoso32_tn);
        int w = u8g2->getStrWidth(data.currentTime.c_str());
        u8g2->setCursor(64 - w/2, 48);
        u8g2->print(data.currentTime);

        // Inset Bar
        u8g2->drawRFrame(12, 54, 104, 6, 2);
        int barW = map((int)constrain(data.tripDistance/1000.0, 0, 100), 0, 100, 0, 100);
        u8g2->drawBox(14, 56, barW, 2);
    }
};

#endif
