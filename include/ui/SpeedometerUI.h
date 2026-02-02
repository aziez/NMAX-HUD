#ifndef SPEEDO_UI_H
#define SPEEDO_UI_H

#include "BaseUI.h"

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
    d->setFont(FONT_MAIN);

    int speed = data.currentSpeed;
    int speedX =
      (speed < 10)  ? 50 :
      (speed < 100) ? 36 :
                      22;

    d->setCursor(speedX, 54);
    d->print(speed);

    /* ================= UNIT ================= */
    d->setFont(u8g2_font_helvB10_tr);
    d->setCursor(speedX + 56, 38);
    d->print("KM");
    d->setCursor(speedX + 56, 48);
    d->print("/H");

    /* ================= SPEED BAR ================= */
    int bar = constrain(map(speed, 0, 120, 0, 44), 0, 44);
    if (speed > 0) {
      d->drawBox(2, 56 - bar, 4, bar);
      d->drawBox(122, 56 - bar, 4, bar);
    }

    /* ================= SEPARATOR ================= */
    d->drawLine(0, 56, 128, 56);

    /* ================= GPS STATE ================= */
    d->setFont(FONT_TINY);

    if (!data.isGpsFixed) {
      if (blink) {
        d->setCursor(30, 63);
        d->print("GPS SEARCHING");
      }
      return;
    }

    /* ================= FOOTER ================= */
    d->setCursor(2, 63);
    d->print("TRIP ");
    d->print(data.tripDistance / 1000.0, 1);
    d->print("km");

    d->setCursor(88, 63);
    d->print("MAX ");
    d->print(data.maxSpeed);
  }
};

#endif
