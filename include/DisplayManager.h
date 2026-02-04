#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "GlobalTypes.h"
#include "Config.h"

// UI Modules
#include "ui/SpeedometerUI.h"
#include "ui/NavigationUI.h"
#include "ui/GameUI.h"
#include "ui/MusicUI.h"
#include "ui/NotificationUI.h"
#include "ui/RobotUI.h"
#include "ui/WifiUI.h"
#include "ui/CompassUI.h"
#include "ui/ClockUI.h"
#include "ui/MenuUI.h"

struct AnimState {
  bool blink;
  int frame;
  int scrollX;
};

class DisplayManager {
public:
  DisplayManager();

  void begin();
  void setBrightness(int val);

  void clear();
  void show();
  void updateAnimations();

  // Screens
  void showWelcome();
  void showBootCheck(String item, String status, int progress);

  void renderSpeedometer(DashboardData data, MusicData music);
  void renderNavigation(NavigationData nav, int speed);
  void renderCompass(DashboardData data);
  void renderGame(GameState state, int score, int bikeY, int obstacleX, int roadLineX);
  void renderMusic(MusicData music);
  void renderNotification(NotificationData notif);
  void renderRobotEyes();
  void renderWiFi(OTAData ota);
  void renderClock(DashboardData data);
  void renderMenu(int selectedIndex, DashboardData data);

  void renderToast(String msg);
  void renderFindBike();

private:
  // Display
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

  // UI Modules
  SpeedometerUI* uiSpeedo;
  CompassUI* uiCompass;
  NavigationUI* uiNav;
  GameUI* uiGame;
  MusicUI* uiMusic;
  NotificationUI* uiNotif;
  RobotUI* uiRobot;
  WifiUI* uiWifi;
  ClockUI* uiClock;
  MenuUI* uiMenu;

  // Animation State
  AnimState anim;
  unsigned long tBlink, tFrame, tScroll;

  // Robot State
  bool robotBlink;
  unsigned long tRobotBlink;
  int robotEyeX;
  unsigned long tRobotLook;
  RobotEmotion robotEmotion;
  unsigned long tRobotEmotion;

  // Helpers
  void renderMusicOverlay(MusicData music);
};

#endif
