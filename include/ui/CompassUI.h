#ifndef COMPASS_UI_H
#define COMPASS_UI_H

#include "BaseUI.h"
#include <math.h>

class CompassUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(const DashboardData& data, bool blink) {

    /* ================= STATUS BAR ================= */
    drawStatusBar(
      data.customName,
      data.satellites,
      data.batteryPhone,
      data.currentTime,
      data.isBtConnected,
      blink
    );

    d->setDrawColor(1);

    /* ================= DIRECTION TEXT ================= */
    // Indonesia direction labels
    static const char* DIR[] = {"U", "TL", "T", "TG", "S", "BD", "B", "BL"};
    int dirIndex = ((int)(data.heading + 22.5) / 45) % 8;

    d->setFont(u8g2_font_logisoso34_tf);
    int dirW = d->getStrWidth(DIR[dirIndex]);
    d->setCursor(64 - (dirW / 2), 48);
    d->print(DIR[dirIndex]);

    /* ================= DEGREE ================= */
    d->setFont(FONT_TINY);
    d->setCursor(96, 24);
    d->print((int)data.heading);
    d->drawStr(114, 24, "°");

    /* ================= COMPASS RING ================= */
    const int cx = 24;
    const int cy = 40;
    const int r  = 17;

    d->drawCircle(cx, cy, r);
    d->drawCircle(cx, cy, r - 1); // double ring for clarity

    // Static North label
    d->setFont(FONT_MICRO);
    d->drawStr(cx - 3, cy - r - 2, "N");

    /* ================= NORTH INDICATOR ================= */
    // Heading: 0 = North, 90 = East
    // North relative to rider
    float angle = (270.0 - data.heading) * DEG_TO_RAD;

    int nx = cx + (r - 4) * cos(angle);
    int ny = cy + (r - 4) * sin(angle);

    d->drawDisc(nx, ny, 3);
  }
};

#endif
