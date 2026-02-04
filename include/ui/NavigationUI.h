#ifndef NAV_UI_H
#define NAV_UI_H

#include "BaseUI.h"

class NavigationUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(const NavigationData& nav, int speed, int animFrame) {

    d->setDrawColor(1);

    /* ================= ICON AREA ================= */
    // Inset from edge
    d->drawRFrame(4, 4, 50, 50, 4);
    drawNavIcon(29, 29, nav.iconType, 36);

    /* ================= DISTANCE ================= */
    if (nav.distance.length()) {
      d->setFont(u8g2_font_logisoso24_tf);
      int w = d->getStrWidth(nav.distance.c_str());
      d->setCursor(94 - w / 2, 28);
      d->print(nav.distance);

      d->setFont(FONT_TINY);
      d->setCursor(94 + w / 2 + 2, 28);
      d->print(nav.unit);
    }

    /* ================= ACTION ================= */
    d->setFont(u8g2_font_helvB08_tr);
    String action = nav.action.length() ? nav.action : "STRAIGHT";
    int aw = d->getStrWidth(action.c_str());
    d->setCursor(94 - aw / 2, 42);
    d->print(action);

    /* ================= STREET ================= */
    d->setFont(FONT_TINY);
    d->setCursor(65, 54);
    d->print(nav.street.substring(0, 10));

    /* ================= BOTTOM BAR ================= */
    d->drawLine(8, 56, 120, 56);
    d->setFont(FONT_MICRO);

    if (nav.eta.length()) {
      d->setCursor(10, 63);
      d->print("ETA " + nav.eta);
    }

    d->setCursor(95, 63);
    d->printf("%d KM/H", speed);
  }

private:
  void drawNavIcon(int cx, int cy, int type, int size) {
    int s = size / 2;
    d->setDrawColor(1);
    switch (type) {
      case 0: d->drawTriangle(cx, cy - s, cx - s / 2, cy, cx + s / 2, cy); d->drawBox(cx - s / 6, cy, s / 3, s); break;
      case 1: d->drawTriangle(cx - s, cy, cx, cy - s, cx, cy + s); d->drawBox(cx, cy - s / 4, s, s / 2); break;
      case 2: d->drawTriangle(cx + s, cy, cx, cy - s, cx, cy + s); d->drawBox(cx - s, cy - s / 4, s, s / 2); break;
      default: d->setFont(u8g2_font_logisoso20_tf); d->setCursor(cx - 8, cy + 8); d->print("?"); break;
    }
  }
};

#endif
