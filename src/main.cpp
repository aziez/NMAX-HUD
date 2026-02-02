#include <Arduino.h>
#include <ESP32Time.h>
#include <Preferences.h>

#include "Config.h"
#include "GlobalTypes.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "BluetoothManager.h"
#include "GpsManager.h"
#include "NetworkManager.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

/* ================= OBJECTS ================= */
DisplayManager    dispMgr;
InputManager      inputMgr;
BluetoothManager  btMgr;
GpsManager        gpsMgr;
NetworkManager    netMgr;
ESP32Time         rtc;
Preferences       preferences;

/* ================= DATA ================= */
DashboardData     dashData;
NavigationData    navData;
NotificationData  notifData;
MusicData         musicData;
OTAData           otaData;

SystemMode currentMode  = MODE_SPEEDO;
SystemMode previousMode = MODE_SPEEDO;

/* ================= GAME ================= */
GameState gameState = GAME_READY;
int gameScore = 0;
int bikeY = 40;
int obstacleX = 128;
int roadLineX = 0;
bool isJumping = false;

/* ================= TIMING ================= */
unsigned long lastDrawTime = 0;
unsigned long lastMusicInteraction = 0;

/* ================= FLAGS ================= */
bool bootCounterReset = false;
SystemMode lastLoopMode = MODE_SPEEDO;

/* ================= PROTOTYPES ================= */
void updateGameLogic();
void processCommand(int cmd);
void switchMode();

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  /* ==== DISPLAY ==== */
  dispMgr.begin();
  dispMgr.setBrightness(255);

  /* ==== BOOT PROTECTION ==== */
  preferences.begin("boot_cfg", false);
  int bootCount = preferences.getInt("crash_cnt", 0) + 1;
  preferences.putInt("crash_cnt", bootCount);

  if (bootCount >= 3) {
    dispMgr.renderToast("EMERGENCY OTA");
    dispMgr.show();

    preferences.putInt("crash_cnt", 0);
    preferences.end();

    netMgr.begin();
    netMgr.startAP();
    currentMode = MODE_WIFI;
    otaData.isUpdating = false;
    delay(2000);
    return;
  }
  preferences.end();

  /* ==== BOOT UI ==== */
  dispMgr.showWelcome();
  delay(1500);

  /* ==== LOAD SETTINGS ==== */
  dispMgr.showBootCheck("STORAGE", "LOAD", 20);
  preferences.begin("nmax_config", true);
  dashData.customName = preferences.getString("name", DEFAULT_NAME);
  dispMgr.setBrightness(preferences.getInt("bri", 255));
  preferences.end();

  /* ==== MODULE INIT ==== */
  inputMgr.begin();
  inputMgr.bindMode(&currentMode);
  dispMgr.showBootCheck("CONTROL", "OK", 40);

  dispMgr.showBootCheck("GPS", "INIT", 60);
  gpsMgr.begin(&rtc);

  dispMgr.showBootCheck("NETWORK", "START", 80);
  btMgr.begin(BT_DEVICE_NAME, &preferences, &rtc);
  btMgr.bindMode(&currentMode, &previousMode);
  netMgr.begin();

  dispMgr.showBootCheck("SYSTEM", "READY", 100);
  delay(800);

  dashData.batteryPhone = -1;
  dashData.isBtConnected = false;
}

/* ================= LOOP ================= */
void loop() {

  /* ==== RESET CRASH COUNTER ==== */
  if (!bootCounterReset && millis() > 5000) {
    preferences.begin("boot_cfg", false);
    preferences.putInt("crash_cnt", 0);
    preferences.end();
    bootCounterReset = true;
  }

  /* ==== INPUT & DATA ==== */
  inputMgr.update();
  gpsMgr.update(&dashData);
  dashData.currentTime = rtc.getTime("%H:%M");

  btMgr.update(&navData, &notifData, &dashData, &musicData, &currentMode);

  /* ==== MUSIC AUTO EXIT ==== */
//   if (currentMode == MODE_MUSIC && lastLoopMode != MODE_MUSIC)
//     lastMusicInteraction = millis();

//   if (currentMode == MODE_MUSIC && millis() - lastMusicInteraction > 5000)
//     currentMode = MODE_SPEEDO;

  lastLoopMode = currentMode;

  /* ==== SERIAL COMMAND ==== */
  if (Serial.available()) {
    if (Serial.readStringUntil('\n') == "ota")
      currentMode = MODE_WIFI;
  }

  /* ==== WIFI MODE ==== */
  if (currentMode == MODE_WIFI) {
    if (!netMgr.isActive()) {
      dispMgr.renderToast("WIFI MODE");
      dispMgr.show();
      btMgr.end();
      netMgr.startAP();
      otaData.isUpdating = false;
    }

    netMgr.handle();
    netMgr.updateUI(&otaData);

    if (millis() - lastDrawTime > 250) {
      lastDrawTime = millis();
      dispMgr.clear();
      dispMgr.renderWiFi(otaData);
      dispMgr.show();
    }

    if (!netMgr.isActive()) {
      btMgr.begin(BT_DEVICE_NAME, &preferences, &rtc);
      btMgr.bindMode(&currentMode, &previousMode);
      currentMode = MODE_SPEEDO;
      dispMgr.renderToast("WIFI OFF");
    }
    return;
  }

  /* ==== GAME INPUT ==== */
  if (currentMode == MODE_GAME && inputMgr.isGameJump()) {
    if (gameState == GAME_READY) {
      gameState = GAME_PLAYING;
      gameScore = 0;
      obstacleX = 128;
      bikeY = 40;
    } else if (gameState == GAME_PLAYING && !isJumping) {
      isJumping = true;
    } else if (gameState == GAME_OVER) {
      gameState = GAME_READY;
    }
  }

  /* ==== COMMAND ==== */
  int cmd = inputMgr.getCommand();
  if (cmd > 0) processCommand(cmd);

  /* ==== DRAW ==== */
  if (millis() - lastDrawTime > DRAW_INTERVAL) {
    lastDrawTime = millis();

    if (currentMode == MODE_GAME && gameState == GAME_PLAYING)
      updateGameLogic();

    dispMgr.clear();
    dispMgr.updateAnimations();

    switch (currentMode) {
      case MODE_SPEEDO:    dispMgr.renderSpeedometer(dashData, musicData); break;
      case MODE_COMPASS:   dispMgr.renderCompass(dashData); break;
      case MODE_NAVIGASI:  dispMgr.renderNavigation(navData, dashData.currentSpeed); break;
      case MODE_NOTIFIKASI:dispMgr.renderNotification(notifData); break;
      case MODE_MUSIC:     dispMgr.renderMusic(musicData); break;
      case MODE_GAME:      dispMgr.renderGame(gameState, gameScore, bikeY, obstacleX, roadLineX); break;
      case MODE_ROBOT:     dispMgr.renderRobotEyes(); break;
      case MODE_FIND_BIKE: dispMgr.renderFindBike(); break;
      default:             dispMgr.renderSpeedometer(dashData, musicData); break;
    }

    dispMgr.show();
  }

  /* ==== NOTIF TIMEOUT ==== */
  if (currentMode == MODE_NOTIFIKASI &&
      millis() - notifData.startTime > 8000) {
    notifData.isActive = false;
    currentMode = MODE_SPEEDO;
  }
}

/* ================= HELPERS ================= */
void processCommand(int cmd) {

     if (currentMode == MODE_MUSIC) {
        switch (cmd) {
        case 1:
            btMgr.sendCommand("CMD:NEXT");
            dispMgr.renderToast("NEXT");
            break;

        case 2:
            btMgr.sendCommand("CMD:PLAYPAUSE");
            musicData.isPlaying = !musicData.isPlaying;
            dispMgr.renderToast(musicData.isPlaying ? "PAUSE" : "PLAY");
            break;

        case 3:
            btMgr.sendCommand("CMD:PREV");
            dispMgr.renderToast("PREV");
            break;

        case 99: // Long press = keluar
            currentMode = MODE_SPEEDO;
            dispMgr.renderToast("EXIT MUSIC");
            break;
        }
        lastMusicInteraction = millis();
        return;
    }

    if (cmd == 99) {
        previousMode = currentMode;
        currentMode = MODE_WIFI;
        return;
    }

    if (currentMode == MODE_GAME) {
        if (cmd == 4) {
        currentMode = MODE_SPEEDO;
        gameState = GAME_READY;
        dispMgr.renderToast("EXIT GAME");
        }
        return;
    }

    if (currentMode == MODE_NOTIFIKASI) {
        notifData.isActive = false;
        currentMode = MODE_SPEEDO;
        return;
    }

    switch (cmd) {
    case 1:
      btMgr.sendCommand("CMD:NEXT");
      dispMgr.renderToast("NEXT");
      currentMode = MODE_MUSIC;
      lastMusicInteraction = millis();
      break;

    case 2:
      btMgr.sendCommand("CMD:PLAYPAUSE");
      musicData.isPlaying = !musicData.isPlaying;
      dispMgr.renderToast(musicData.isPlaying ? "PAUSE" : "PLAY");
      currentMode = MODE_MUSIC;
      lastMusicInteraction = millis();
      break;

    case 3:
      btMgr.sendCommand("CMD:PREV");
      dispMgr.renderToast("PREV");
      currentMode = MODE_MUSIC;
      lastMusicInteraction = millis();
      break;

    case 4:
      switchMode();
      break;
  }
}

void switchMode() {
  if (currentMode == MODE_SPEEDO) currentMode = MODE_COMPASS;
  else if (currentMode == MODE_COMPASS) currentMode = MODE_ROBOT;
  else if (currentMode == MODE_ROBOT) { currentMode = MODE_GAME; gameState = GAME_READY; }
  else currentMode = MODE_SPEEDO;

  dispMgr.renderToast(
    currentMode == MODE_COMPASS ? "COMPASS" :
    currentMode == MODE_ROBOT   ? "ROBOT" :
    currentMode == MODE_GAME    ? "GAME"  : "SPEEDO"
  );
}

void updateGameLogic() {
  if (gameState != GAME_PLAYING) return;

  if (isJumping) {
    bikeY -= 3;
    if (bikeY < 10) isJumping = false;
  } else if (bikeY < 40) {
    bikeY += 3;
  }

  obstacleX -= 5;
  if (obstacleX < -15) {
    obstacleX = 128;
    gameScore++;
  }

  roadLineX -= 4;
  if (roadLineX < 0) roadLineX = 20;

  if (obstacleX < 35 && obstacleX > 10 && bikeY > 30)
    gameState = GAME_OVER;
}
/* ================= UI CLASSES ================= */