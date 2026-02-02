#ifndef BASEUI_H
#define BASEUI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "GlobalTypes.h"
#include "Config.h"

/* ================= FONT SYSTEM ================= */
#define FONT_MICRO u8g2_font_micro_tr
#define FONT_TINY  u8g2_font_profont10_tf
#define FONT_SMALL u8g2_font_helvB10_tf
#define FONT_MAIN  u8g2_font_logisoso42_tn

/* ================= BASE UI ================= */
class BaseUI {
protected:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C* d;

public:
  explicit BaseUI(U8G2_SH1106_128X64_NONAME_F_HW_I2C* display)
    : d(display) {}

  /* ================= STATUS BAR ================= */
  void drawStatusBar(
    const String& title,
    int sat,
    int bat,
    const String& time,
    bool btConnected,
    bool blink
  ) {
    // Background (soft header)
    d->setDrawColor(1);
    d->drawBox(0, 0, 128, 10);

    d->setDrawColor(0);
    d->setFont(FONT_MICRO);

    // Title
    d->setCursor(2, 8);
    d->print(title.substring(0, 10));

    // Bluetooth Indicator
    if (btConnected) {
      d->drawDisc(50, 5, 2);
    }

    // Satellite / GPS
    d->setCursor(58, 8);
    if (sat > 0) {
      d->print(sat);
    } else if (blink) {
      d->print("?");
    }

    // Battery
    if (bat >= 0) {
      d->drawFrame(72, 2, 14, 6);
      int fill = map(bat, 0, 100, 0, 12);
      if (fill > 0) d->drawBox(73, 3, fill, 4);
    } else {
      d->drawStr(72, 8, "--");
    }

    // Time
    d->setFont(FONT_TINY);
    d->setCursor(96, 8);
    d->print(time);

    d->setDrawColor(1);
  }

  /* ================= PROGRESS BAR ================= */
  void drawProgressBar(int x, int y, int w, int h, int percent) {
    percent = constrain(percent, 0, 100);
    d->drawFrame(x, y, w, h);

    int fill = map(percent, 0, 100, 0, w - 2);
    if (fill > 0) {
      d->drawBox(x + 1, y + 1, fill, h - 2);
    }
  }

  /* ================= MARQUEE TEXT ================= */
  void drawMarquee(
    int x,
    int y,
    int width,
    const String& text,
    int scrollX
  ) {
    d->setFont(FONT_TINY);
    int textW = d->getStrWidth(text.c_str());

    // Clip area
    d->setClipWindow(x, y - 9, x + width, y + 2);

    if (textW <= width) {
      d->setCursor(x, y);
      d->print(text);
    } else {
      d->setCursor(x + scrollX, y);
      d->print(text);
    }

    d->setMaxClipWindow();
  }

  /* ================= DIVIDER ================= */
  void drawDivider(int y) {
    d->drawHLine(0, y, 128);
  }

  /* ================= CENTER TEXT ================= */
  void drawCenteredText(int y, const String& text) {
    int w = d->getStrWidth(text.c_str());
    d->setCursor(64 - (w / 2), y);
    d->print(text);
  }
};

#endif
