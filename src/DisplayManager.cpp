#include "DisplayManager.h"
#include <Wire.h>

DisplayManager::DisplayManager()
  : u8g2(U8G2_R2, U8X8_PIN_NONE) {

  anim = { false, 0, 0 };
  tBlink = tFrame = tScroll = 0;

  robotBlink = false;
  tRobotBlink = 0;
  robotEyeX = 0;
  tRobotLook = 0;

  uiSpeedo  = new SpeedometerUI(&u8g2);
  uiCompass = new CompassUI(&u8g2);
  uiNav     = new NavigationUI(&u8g2);
  uiGame    = new GameUI(&u8g2);
  uiMusic   = new MusicUI(&u8g2);
  uiNotif   = new NotificationUI(&u8g2);
  uiRobot   = new RobotUI(&u8g2);
  uiWifi    = new WifiUI(&u8g2);
  uiClock   = new ClockUI(&u8g2);
  uiMenu    = new MenuUI(&u8g2);
}

void DisplayManager::begin() {
  u8g2.begin();
  Wire.setClock(400000); // 400kHz I2C for faster display updates
  u8g2.enableUTF8Print();
  u8g2.setContrast(DEFAULT_BRIGHTNESS);

    robotBlink = false;
    robotEyeX = 0;
    robotEmotion = ROBOT_NEUTRAL;

    tRobotBlink = millis();
    tRobotLook = millis();
    tRobotEmotion = millis();
}

void DisplayManager::setBrightness(int val) {
  u8g2.setContrast(val);
}

void DisplayManager::clear() {
  u8g2.clearBuffer();
}

void DisplayManager::show() {
  u8g2.sendBuffer();
}

/* ================= ANIMATION CONTROLLER ================= */

void DisplayManager::updateAnimations() {
  unsigned long now = millis();

  /* ===== GLOBAL FRAME ===== */
  if (now - tFrame > 120) {
    anim.frame++;
    tFrame = now;
  }

  /* ===== BLINK (GLOBAL) ===== */
  if (now - tBlink > 3000) {
    anim.blink = true;
    tBlink = now;
  }
  if (anim.blink && now - tBlink > 120) {
    anim.blink = false;
  }

  /* ===== MARQUEE ===== */
  if (now - tScroll > 80) {
    anim.scrollX--;
    if (anim.scrollX < -128) anim.scrollX = 0;
    tScroll = now;
  }

  /* ================= ROBOT ================= */

  // Blink mata robot
  if (now - tRobotBlink > random(2500, 5000)) {
    robotBlink = true;
    tRobotBlink = now;
  }
  if (robotBlink && now - tRobotBlink > 120) {
    robotBlink = false;
  }

  // Gerak mata kiri kanan
  if (now - tRobotLook > 2000) {
    robotEyeX = random(-6, 7); // geser dikit
    tRobotLook = now;
  }

  // Ganti ekspresi random
  if (now - tRobotEmotion > 4000) {
    robotEmotion = (RobotEmotion)random(0, 6);
    tRobotEmotion = now;
  }
}


/* ================= STATIC SCREENS ================= */

void DisplayManager::showWelcome() {
  u8g2.clearBuffer();

  u8g2.drawRFrame(0, 0, 128, 64, 4);
  u8g2.setFont(u8g2_font_logisoso32_tn);

  int w = u8g2.getStrWidth("NMAX");
  u8g2.setCursor(64 - w / 2, 40);
  u8g2.print("NMAX");

  u8g2.setFont(u8g2_font_profont10_tf);
  String sub = "SMART HUD SYSTEM v2.0";
  w = u8g2.getStrWidth(sub.c_str());
  u8g2.setCursor(64 - w / 2, 55);
  u8g2.print(sub);

  u8g2.sendBuffer();
}

void DisplayManager::showBootCheck(String item, String status, int progress) {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_helvB08_tr);
  u8g2.drawBox(0, 0, 128, 12);
  u8g2.setDrawColor(0);
  u8g2.setCursor(2, 9);
  u8g2.print("SYSTEM CHECK...");
  u8g2.setDrawColor(1);

  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.setCursor(5, 30);
  u8g2.print(item);

  u8g2.setCursor(120 - u8g2.getStrWidth(status.c_str()), 30);
  u8g2.print(status);

  u8g2.drawFrame(10, 50, 108, 6);
  u8g2.drawBox(11, 51, map(progress, 0, 100, 0, 106), 4);

  u8g2.sendBuffer();
}

/* ================= MAIN RENDER ================= */

void DisplayManager::renderSpeedometer(DashboardData data, MusicData music) {
  uiSpeedo->render(data, anim.blink);

  if (music.isPlaying) {
    renderMusicOverlay(music);
  }
}

void DisplayManager::renderMusicOverlay(MusicData music) {
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 0, 128, 11);
  u8g2.setDrawColor(1);
  u8g2.drawLine(0, 11, 128, 11);

  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  u8g2.drawGlyph(2, 9, 237);

  u8g2.setFont(u8g2_font_profont10_tf);
  String txt = music.title;
  if (music.artist.length()) txt += " - " + music.artist;

  int w = u8g2.getStrWidth(txt.c_str());
  int x = (w > 110) ? anim.scrollX + 20 : 14;

  u8g2.setCursor(x, 8);
  u8g2.print(txt);
}

void DisplayManager::renderNavigation(NavigationData nav, int speed) {
  uiNav->render(nav, speed, anim.scrollX);
}

void DisplayManager::renderCompass(DashboardData data) {
  uiCompass->render(data, anim.blink);
}

void DisplayManager::renderGame(GameState state, int score, int bikeY, int obstacleX, int roadLineX) {
  uiGame->render(state, score, bikeY, obstacleX, roadLineX, anim.blink);
}

void DisplayManager::renderMusic(MusicData music) {
  uiMusic->render(music, anim.frame);
}

void DisplayManager::renderNotification(NotificationData notif) {
  uiNotif->render(notif);
}

void DisplayManager::renderRobotEyes() {
  clear();

  uiRobot->render(
    robotBlink,
    robotEyeX,
    robotEmotion,
    anim.frame
  );

  show();
}

void DisplayManager::renderWiFi(OTAData ota) {
  uiWifi->render(ota, anim.blink);
}

void DisplayManager::renderClock(DashboardData data) {
  uiClock->render(data, data.clockStyle, anim.blink);
}

void DisplayManager::renderMenu(int selectedIndex, DashboardData data) {
  uiMenu->render(selectedIndex, data);
}

/* ================= UTIL ================= */

void DisplayManager::renderToast(String msg) {
  u8g2.setDrawColor(1);
  u8g2.drawRBox(5, 15, 118, 34, 4);
  u8g2.setDrawColor(0);
  u8g2.drawRBox(7, 17, 114, 30, 3);
  
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_helvB10_tf);
  int w = u8g2.getStrWidth(msg.c_str());
  u8g2.setCursor(64 - w / 2, 38);
  u8g2.print(msg);
}

void DisplayManager::renderFindBike() {
  if (anim.blink) {
    u8g2.drawBox(0, 0, 128, 64);
    u8g2.setDrawColor(0);
  }

  u8g2.setFont(u8g2_font_logisoso24_tf);
  u8g2.drawStr(10, 45, "I'M HERE!");
  u8g2.setDrawColor(1);
}
