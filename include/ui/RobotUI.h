#ifndef ROBOT_UI_H
#define ROBOT_UI_H

#include "BaseUI.h"

class RobotUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(bool isBlinking, int eyeX, RobotEmotion emotion, int animFrame) {

    d->setDrawColor(1);

    /* ================= FACE FRAME ================= */
    d->drawRFrame(0, 0, 128, 64, 6);

    /* ================= EYES ================= */
    drawEyes(isBlinking, eyeX);

    /* ================= MOUTH ================= */
    drawMouth(emotion, animFrame);

    /* ================= TEARS / EFFECT ================= */
    if (emotion == ROBOT_CRY) {
      drawTears(animFrame);
    }
  }

private:

  /* ---------- EYES ---------- */
  void drawEyes(bool blink, int eyeX) {
    int h = blink ? 3 : 26;
    int y = blink ? 30 : 18;

    d->drawRBox(22 + eyeX, y, 28, h, 4);
    d->drawRBox(78 + eyeX, y, 28, h, 4);
  }

  /* ---------- MOUTH ---------- */
  void drawMouth(RobotEmotion emo, int frame) {

    int cx = 64;
    int y  = 48;

    switch (emo) {

      case ROBOT_NEUTRAL:
        d->drawHLine(cx - 10, y, 20);
        break;

      case ROBOT_HAPPY:
        d->drawArc(cx, y - 2, 10, 0, 180);
        break;

      case ROBOT_LAUGH: {
        int open = (frame % 2 == 0) ? 6 : 10;
        d->drawRBox(cx - 12, y - open / 2, 24, open, 3);
        break;
      }

      case ROBOT_SAD:
        d->drawArc(cx, y + 6, 10, 180, 104);
        break;

      case ROBOT_CRY:
        d->drawArc(cx, y + 6, 10, 180, 104);
        break;

      case ROBOT_ANGRY:
        d->drawHLine(cx - 12, y + 2, 24);
        d->drawLine(cx - 12, y + 2, cx - 6, y - 2);
        d->drawLine(cx + 12, y + 2, cx + 6, y - 2);
        break;
    }
  }

  /* ---------- TEARS ---------- */
  void drawTears(int frame) {
    int dropY = 36 + (frame % 6);
    d->drawDisc(32, dropY, 2);
    d->drawDisc(96, dropY, 2);
  }
};

#endif
