#ifndef SPEEDO_UI_H
#define SPEEDO_UI_H

#include "BaseUI.h"
#include "TripManager.h"

class SpeedometerUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(DashboardData data, bool blink) {

    /* ================= STATUS BAR ================= */
    drawStatusBar(
      data.customName,
      data.satellites,
      data.batteryPhone,
      data.currentTime,
      data.isBtConnected,
      blink
    );

    /* ================= SPEED ================= */
    d->setDrawColor(1);
    d->setFont(u8g2_font_logisoso42_tn); // Bigger font for clear view

    int speed = data.currentSpeed;
    String speedStr = String(speed);
    int speedW = d->getStrWidth(speedStr.c_str());
    int speedX = 64 - speedW/2;
    
    // Position speed 2px higher to give footer more room
    d->setCursor(speedX, 50);
    d->print(speedStr);

    /* ================= UNIT ================= */
    d->setFont(u8g2_font_helvB08_tr);
    // Move "KM/H" to be more visible, not hidden by speed
    d->setCursor(95, 40); 
    d->print("KM/H");

    /* ================= DECORATION ================= */
    // Subtle arcs with 6px side padding
    d->drawCircle(64, 32, 32, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    d->drawCircle(64, 32, 34, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);

    /* ================= FOOTER DATA ================= */
    // Footer line with 8px margin
    d->drawLine(8, 54, 120, 54);
    d->setFont(u8g2_font_profont10_tf);
    
    // Trip Distance
    d->setCursor(10, 63);
    d->printf("TRIP: %.1f", data.tripDistance/1000.0);

    // Ride Time
    String rt = TripManager::getFormattedTime(data.tripSeconds);
    int rtW = d->getStrWidth(rt.c_str());
    d->setCursor(118 - rtW, 63);
    d->print(rt);
  }
};

#endif
