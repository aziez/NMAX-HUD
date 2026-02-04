#ifndef ROBOT_UI_H
#define ROBOT_UI_H

#include "BaseUI.h"

class RobotUI : public BaseUI {
public:
  using BaseUI::BaseUI;

  void render(bool isBlinking, int eyeX, RobotEmotion emotion, int animFrame) {

    d->setDrawColor(1);

    /* ================= FACE FRAME ================= */
    // Inset 4px to avoid bezel
    d->drawRFrame(4, 4, 120, 56, 8);

    /* ================= EYES ================= */
    drawEyes(isBlinking, eyeX);

    /* ================= MOUTH ================= */
    drawMouth(emotion, animFrame);

    /* ================= TEARS ================= */
    if (emotion == ROBOT_CRY) {
      drawTears(animFrame);
    }
  }

private:
  void drawEyes(bool blink, int eyeX) {
    int h = blink ? 3 : 22;
    int y = blink ? 32 : 20;

    d->drawRBox(30 + eyeX, y, 24, h, 4);
    d->drawRBox(74 + eyeX, y, 24, h, 4);
  }

  void drawMouth(RobotEmotion emo, int frame) {
    int cx = 64;
    int y  = 48;
    switch (emo) {
      case ROBOT_NEUTRAL: d->drawHLine(cx - 10, y, 20); break;
      case ROBOT_HAPPY:   d->drawArc(cx, y - 2, 10, 0, 127); break;
      case ROBOT_SAD:     d->drawArc(cx, y + 6, 10, 128, 255); break;
      case ROBOT_CRY:     d->drawArc(cx, y + 6, 10, 128, 255); break;
      case ROBOT_LAUGH:   d->drawRBox(cx - 12, y - 4, 24, 8, 3); break;
      case ROBOT_ANGRY:   d->drawHLine(cx - 12, y + 2, 24); d->drawLine(cx - 12, y + 2, cx - 6, y - 2); d->drawLine(cx + 12, y + 2, cx + 6, y - 2); break;
    }
  }

  void drawTears(int frame) {
    int dropY = 38 + (frame % 6);
    d->drawDisc(38, dropY, 2);
    d->drawDisc(90, dropY, 2);
  }
};

#endif
