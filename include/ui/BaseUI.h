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
    // Floating Status Bar (6px padding from left/right)
    d->setDrawColor(1);
    d->drawRBox(6, 1, 116, 11, 2);

    d->setDrawColor(0);
    d->setFont(FONT_MICRO);

    // Title (shifted right)
    d->setCursor(10, 9);
    d->print(title.substring(0, 8));

    // Bluetooth Indicator
    if (btConnected) {
      d->drawDisc(54, 6, 2);
    }

    // Satellite / GPS
    d->setCursor(62, 9);
    if (sat > 0) {
      if (sat < 10) d->print(" "); // align single digit
      d->print(sat);
    } else if (blink) {
      d->print(" ?");
    }

    // Battery
    d->drawFrame(80, 4, 12, 5);
    if (bat >= 0) {
      int fill = map(bat, 0, 100, 0, 10);
      if (fill > 0) d->drawBox(81, 5, fill, 3);
    }

    // Time (shifted left)
    d->setFont(FONT_TINY);
    d->setCursor(96, 9);
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
