#ifndef MUSIC_UI_H
#define MUSIC_UI_H

#include "BaseUI.h"

class MusicUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(const MusicData& music, int animFrame) {

    d->setDrawColor(1);

    /* ================= HEADER ================= */
    d->drawHLine(0, 12, 128);

    // Music icon
    d->setFont(u8g2_font_open_iconic_all_1x_t);
    d->drawGlyph(2, 10, 237); // music note

    d->setFont(FONT_TINY);
    d->setCursor(16, 10);
    d->print("MUSIC");

    /* ================= TITLE ================= */
    d->setFont(FONT_SMALL);
    drawMarquee(
      6,
      30,
      116,
      music.title.length() ? music.title : "Unknown Title",
      animFrame * -3
    );

    /* ================= ARTIST ================= */
    d->setFont(FONT_TINY);
    drawMarquee(
      6,
      44,
      116,
      music.artist.length() ? music.artist : "Unknown Artist",
      animFrame * -2
    );

    /* ================= EQUALIZER ================= */
    int eqX = 6;
    for (int i = 0; i < 4; i++) {
      int h = 6 + (animFrame + i) % 8;
      d->drawBox(eqX + i * 6, 58 - h, 4, h);
    }

    /* ================= PLAY STATE ================= */
    if (music.isPlaying) {
      d->drawTriangle(112, 52, 112, 62, 122, 57);
    } else {
      d->drawBox(112, 52, 3, 10);
      d->drawBox(118, 52, 3, 10);
    }
  }
};

#endif
