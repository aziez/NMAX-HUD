#ifndef GAME_UI_H
#define GAME_UI_H

#include "BaseUI.h"

class GameUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(
    GameState state,
    int score,
    int bikeY,
    int obstacleX,
    int roadLineX,
    bool blink
  ) {
    d->setDrawColor(1);

    /* ================= READY SCREEN ================= */
    if (state == GAME_READY) {
      d->setFont(FONT_SMALL);
      drawCenteredText(24, "CYBER BIKE");

      d->setFont(FONT_TINY);
      drawCenteredText(38, "SCORE " + String(score));

      if (blink) {
        d->setFont(FONT_MICRO);
        drawCenteredText(56, "TOGGLE TO START");
      }
      return;
    }

    /* ================= SCORE ================= */
    d->setFont(FONT_MICRO);
    d->setCursor(100, 10);
    d->print(score);

    /* ================= ROAD ================= */
    for (int i = 0; i < 128; i += 24) {
      int x = (i + roadLineX) % 128;
      d->drawHLine(x, 58, 12);
    }

    /* ================= BIKE (SIMPLIFIED ICON) ================= */
    int wheelY = bikeY + 10;

    // Wheels
    d->drawDisc(18, wheelY, 3);
    d->drawDisc(34, wheelY, 3);

    // Body
    d->drawLine(18, wheelY, 34, wheelY);
    d->drawLine(24, bikeY, 18, wheelY);
    d->drawLine(24, bikeY, 34, wheelY);

    /* ================= OBSTACLE ================= */
    d->drawBox(obstacleX, 46, 10, 10);
    d->drawTriangle(
      obstacleX, 46,
      obstacleX + 5, 42,
      obstacleX + 10, 46
    );

    /* ================= GAME OVER ================= */
    if (state == GAME_OVER) {
      d->setFont(FONT_SMALL);
      drawCenteredText(28, "GAME OVER");

      d->setFont(FONT_TINY);
      drawCenteredText(40, "SCORE " + String(score));

      if (blink) {
        d->setFont(FONT_MICRO);
        drawCenteredText(56, "PRESS TO RETRY");
      }
    }
  }
};

#endif
