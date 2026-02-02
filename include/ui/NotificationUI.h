#ifndef NOTIF_UI_H
#define NOTIF_UI_H

#include "BaseUI.h"

class NotificationUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(const NotificationData& notif) {

    d->setDrawColor(1);
    d->drawRFrame(0, 0, 128, 64, 4);

    /* ================= HEADER ================= */
    d->drawBox(0, 0, 128, 16);

    // App Icon
    drawIcon(4, 8, notif.iconType);

    // App Name
    d->setDrawColor(0);
    d->setFont(u8g2_font_helvB08_tr);
    d->setCursor(22, 12);
    d->print(notif.app.substring(0, 14));

    d->setDrawColor(1);

    /* ================= SENDER ================= */
    d->setFont(u8g2_font_helvB10_tr);
    d->setCursor(6, 32);
    d->print(notif.sender.substring(0, 20));

    /* ================= MESSAGE ================= */
    d->setFont(u8g2_font_helvR08_tr);
    drawWrappedText(6, 44, 116, notif.message);

    /* ================= TIMER ================= */
    long elapsed = millis() - notif.startTime;
    elapsed = constrain(elapsed, 0, 8000);

    int barW = map(elapsed, 0, 8000, 118, 0);
    d->drawBox(5, 58, barW, 3);

    d->setFont(FONT_MICRO);
    d->setCursor(108, 56);
    d->print((8000 - elapsed) / 1000);
    d->print("s");
  }

private:
  /* ---------- ICON (SAFE DRAW) ---------- */
  void drawIcon(int x, int y, int type) {
    int tx = x;
    int ty = y - 6;

    // base icon background (WHITE)
    d->setDrawColor(0);
    d->drawRBox(tx, ty, 12, 12, 3);

    d->setDrawColor(1);

    switch (type) {
      case 1: // WhatsApp
        d->drawCircle(tx + 6, ty + 6, 4);
        d->drawDisc(tx + 8, ty + 8, 1);
        break;

      case 2: // Instagram
        d->drawRFrame(tx + 2, ty + 2, 8, 8, 2);
        d->drawCircle(tx + 6, ty + 6, 2);
        break;

      case 3: // Facebook
        d->setFont(u8g2_font_helvB10_tr);
        d->drawStr(tx + 4, ty + 10, "f");
        break;

      case 4: // Mail
        d->drawFrame(tx + 2, ty + 4, 8, 5);
        d->drawLine(tx + 2, ty + 4, tx + 6, ty + 7);
        d->drawLine(tx + 10, ty + 4, tx + 6, ty + 7);
        break;

      case 6: // Telegram
        d->drawTriangle(tx + 3, ty + 10, tx + 10, ty + 3, tx + 10, ty + 9);
        break;

      default: // Generic
        d->setFont(FONT_MICRO);
        d->drawStr(tx + 4, ty + 9, "!");
        break;
    }

    d->setDrawColor(1);
  }

  /* ---------- TEXT WRAP (2 lines) ---------- */
  void drawWrappedText(int x, int y, int w, const String& txt) {
    String l1 = txt;
    String l2 = "";

    while (d->getStrWidth(l1.c_str()) > w && l1.length() > 0) {
      l2 = l1.substring(l1.length() - 1) + l2;
      l1.remove(l1.length() - 1);
    }

    d->setCursor(x, y);
    d->print(l1);

    if (l2.length()) {
      d->setCursor(x, y + 10);
      d->print(l2.substring(0, 40));
    }
  }
};

#endif
