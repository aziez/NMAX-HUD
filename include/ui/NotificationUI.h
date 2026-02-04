#ifndef NOTIF_UI_H
#define NOTIF_UI_H

#include "BaseUI.h"

class NotificationUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(const NotificationData& notif) {
    d->setDrawColor(1);
    // Inset frame (4px margin)
    d->drawRFrame(4, 4, 120, 56, 4);

    /* ================= HEADER ================= */
    d->drawRBox(4, 4, 120, 14, 2);

    // App Icon
    drawIcon(8, 11, notif.iconType);

    // App Name
    d->setDrawColor(0);
    d->setFont(u8g2_font_helvB08_tr);
    d->setCursor(26, 15);
    d->print(notif.app.substring(0, 12));

    d->setDrawColor(1);

    /* ================= SENDER ================= */
    d->setFont(u8g2_font_helvB10_tr);
    d->setCursor(10, 32);
    d->print(notif.sender.substring(0, 18));

    /* ================= MESSAGE ================= */
    d->setFont(u8g2_font_helvR08_tr);
    drawWrappedText(10, 44, 108, notif.message);

    /* ================= TIMER BAR ================= */
    long elapsed = millis() - notif.startTime;
    int barW = map(constrain(elapsed, 0, 8000), 0, 8000, 112, 0);
    d->drawBox(8, 56, barW, 2);
  }

private:
  void drawIcon(int x, int y, int type) {
    int tx = x; int ty = y - 6;
    d->setDrawColor(0);
    d->drawRBox(tx, ty, 12, 12, 2);
    d->setDrawColor(1);
    d->setFont(FONT_MICRO);
    d->drawStr(tx + 4, ty + 9, "!");
  }

  void drawWrappedText(int x, int y, int w, const String& txt) {
    String l1 = txt.substring(0, 20);
    d->setCursor(x, y);
    d->print(l1);
  }
};

#endif
