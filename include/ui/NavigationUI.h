#ifndef NAV_UI_H
#define NAV_UI_H

#include "BaseUI.h"

class NavigationUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(const NavigationData& nav, int speed, int animFrame) {

    d->setDrawColor(1);

    /* ================= ICON AREA ================= */
    d->drawRFrame(0, 0, 56, 56, 3);
    drawNavIcon(28, 30, nav.iconType, 40);

    /* ================= DISTANCE ================= */
    if (nav.distance.length()) {
      d->setFont(u8g2_font_logisoso24_tf);
      int w = d->getStrWidth(nav.distance.c_str());
      d->setCursor(92 - w / 2, 26);
      d->print(nav.distance);

      d->setFont(FONT_TINY);
      d->setCursor(92 + w / 2 + 2, 26);
      d->print(nav.unit);
    }

    /* ================= ACTION ================= */
    d->setFont(u8g2_font_helvB08_tr);
    String action = nav.action.length()
      ? nav.action.substring(0, 12)
      : "GO STRAIGHT";

    int aw = d->getStrWidth(action.c_str());
    d->setCursor(92 - aw / 2, 40);
    d->print(action);

    /* ================= STREET ================= */
    if (nav.street.length()) {
      d->setFont(FONT_TINY);
      int scroll = (animFrame / 2) % (nav.street.length() * 6);
      d->setClipWindow(60, 42, 126, 52);
      d->setCursor(126 - scroll, 52);
      d->print(nav.street);
      d->setMaxClipWindow();
    }

    /* ================= BOTTOM BAR ================= */
    d->drawHLine(0, 56, 128);
    d->setFont(FONT_MICRO);

    if (nav.eta.length()) {
      d->setCursor(4, 63);
      d->print("ETA ");
      d->print(nav.eta);
    }

    d->setCursor(88, 63);
    d->print(speed);
    d->print("kmh");
  }

private:
  void drawNavIcon(int cx, int cy, int type, int size) {
    int s = size / 2;
    d->setDrawColor(1);

    switch (type) {
      case 0: // straight
        d->drawTriangle(cx, cy - s, cx - s / 2, cy, cx + s / 2, cy);
        d->drawBox(cx - s / 6, cy, s / 3, s);
        break;

      case 1: // left
        d->drawTriangle(cx - s, cy, cx, cy - s, cx, cy + s);
        d->drawBox(cx, cy - s / 4, s, s / 2);
        break;

      case 2: // right
        d->drawTriangle(cx + s, cy, cx, cy - s, cx, cy + s);
        d->drawBox(cx - s, cy - s / 4, s, s / 2);
        break;

      case 3: // U-turn
        d->drawCircle(cx, cy, s / 2);
        d->drawTriangle(cx - s / 2, cy - s, cx - s / 2, cy - s / 2, cx, cy - s / 2);
        break;

      case 4: // roundabout
        d->drawCircle(cx, cy, s / 2);
        d->drawTriangle(cx + s / 2, cy, cx + s, cy - s / 4, cx + s, cy + s / 4);
        break;

      default:
        d->setFont(u8g2_font_logisoso20_tf);
        d->setCursor(cx - 8, cy + 8);
        d->print("?");
        break;
    }
  }
};

#endif
