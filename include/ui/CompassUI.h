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
    static const char* DIR[] = {"U", "TL", "T", "TG", "S", "BD", "B", "BL"};
    int dirIndex = ((int)(data.heading + 22.5) / 45) % 8;

    d->setFont(u8g2_font_logisoso34_tf);
    int dirW = d->getStrWidth(DIR[dirIndex]);
    d->setCursor(72 - (dirW / 2), 52); // Shifted right and down
    d->print(DIR[dirIndex]);

    /* ================= DEGREE ================= */
    d->setFont(u8g2_font_helvB08_tr);
    d->setCursor(102, 38);
    d->printf("%d", (int)data.heading);
    d->drawStr(102 + d->getStrWidth(String((int)data.heading).c_str()) + 2, 38, "o");

    /* ================= COMPASS RING ================= */
    // Moved away from the edge (cx: 24 -> 32)
    const int cx = 32;
    const int cy = 42;
    const int r  = 16;

    d->drawCircle(cx, cy, r);
    d->drawCircle(cx, cy, r - 1);

    // Dynamic North Indicator
    float angle = (270.0 - data.heading) * DEG_TO_RAD;
    int nx = cx + (r - 4) * cos(angle);
    int ny = cy + (r - 4) * sin(angle);
    d->drawDisc(nx, ny, 3);
  }
};

#endif
