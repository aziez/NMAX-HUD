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
#include "TripManager.h"

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
TripManager       tripMgr;

/* ================= DATA ================= */
DashboardData     dashData;
NavigationData    navData;
NotificationData  notifData;
MusicData         musicData;
OTAData           otaData;

/* ================= SYSTEM ================= */
SystemMode currentMode  = MODE_CLOCK;
SystemMode previousMode = MODE_CLOCK;

/* ================= MENU ================= */
int menuIndex = 0;

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

/* ================= PROTOTYPES ================= */
void updateGameLogic();
void handleAction(InputAction action);
void switchMode();

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  /* ==== DISPLAY ==== */
  dispMgr.begin();
  dispMgr.setBrightness(DEFAULT_BRIGHTNESS);

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
  dashData.clockStyle = preferences.getInt("clk_style", 0);
  dashData.brightness = preferences.getInt("bri", DEFAULT_BRIGHTNESS);
  dispMgr.setBrightness(dashData.brightness);
  preferences.end();

  /* ==== MODULE INIT ==== */
  inputMgr.begin();
  inputMgr.bindMode(&currentMode);
  dispMgr.showBootCheck("CONTROL", "OK", 40);

  dispMgr.showBootCheck("GPS", "INIT", 60);
  gpsMgr.begin(&rtc);
  tripMgr.begin(&dashData);

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
  tripMgr.update(&dashData);
  dashData.currentTime = rtc.getTime("%H:%M");

  btMgr.update(&navData, &notifData, &dashData, &musicData, &currentMode);

  /* ==== SERIAL OTA ==== */
  if (Serial.available()) {
    if (Serial.readStringUntil('\n') == "ota") {
      currentMode = MODE_WIFI;
    }
  }

  /* ==== HANDLE INPUT ACTION ==== */
  if (inputMgr.hasAction()) {
    InputAction action = inputMgr.consumeAction();
    handleAction(action);
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

    return;
  }

  /* ==== DRAW ==== */
  if (millis() - lastDrawTime > DRAW_INTERVAL) {
    lastDrawTime = millis();

    if (currentMode == MODE_GAME && gameState == GAME_PLAYING) {
      updateGameLogic();
    }

    dispMgr.clear();
    dispMgr.updateAnimations();

    switch (currentMode) {
      case MODE_SPEEDO:     dispMgr.renderSpeedometer(dashData, musicData); break;
      case MODE_CLOCK:      dispMgr.renderClock(dashData); break;
      case MODE_MENU:       dispMgr.renderMenu(menuIndex, dashData); break;
      case MODE_COMPASS:    dispMgr.renderCompass(dashData); break;
      case MODE_NAVIGASI:   dispMgr.renderNavigation(navData, dashData.currentSpeed); break;
      case MODE_NOTIFIKASI: dispMgr.renderNotification(notifData); break;
      case MODE_MUSIC:      dispMgr.renderMusic(musicData); break;
      case MODE_GAME:       dispMgr.renderGame(gameState, gameScore, bikeY, obstacleX, roadLineX); break;
      case MODE_ROBOT:      dispMgr.renderRobotEyes(); break;
      case MODE_FIND_BIKE:  dispMgr.renderFindBike(); break;
      default:              dispMgr.renderClock(dashData); break;
    }

    dispMgr.show();
  }

  /* ==== NOTIF TIMEOUT ==== */
  if (currentMode == MODE_NOTIFIKASI &&
      millis() - notifData.startTime > NOTIF_DURATION) {
    notifData.isActive = false;
    currentMode = MODE_SPEEDO;
  }
}

/* ================= ACTION HANDLER ================= */
void handleAction(InputAction action) {
  switch (action) {
    /* ===== UI ===== */
    case ACTION_NEXT_SCREEN:
      switchMode();
      break;

    /* ===== MUSIC ===== */
    case ACTION_MUSIC_PLAY_PAUSE:
      btMgr.sendCommand("CMD:PLAYPAUSE");
      musicData.isPlaying = !musicData.isPlaying;
      dispMgr.renderToast(musicData.isPlaying ? "PAUSE" : "PLAY");
      currentMode = MODE_MUSIC;
      break;

    case ACTION_MUSIC_NEXT:
      btMgr.sendCommand("CMD:NEXT");
      dispMgr.renderToast("NEXT");
      currentMode = MODE_MUSIC;
      break;

    case ACTION_MUSIC_PREV:
      btMgr.sendCommand("CMD:PREV");
      dispMgr.renderToast("PREV");
      currentMode = MODE_MUSIC;
      break;

    /* ===== GAME ===== */
    case ACTION_GAME_JUMP:
      if (gameState == GAME_READY) {
        gameState = GAME_PLAYING;
        gameScore = 0;
        obstacleX = 128;
        bikeY = 40;
      } else if (!isJumping) {
        isJumping = true;
      }
      break;

    case ACTION_GAME_EXIT:
      currentMode = MODE_SPEEDO;
      gameState = GAME_READY;
      dispMgr.renderToast("EXIT GAME");
      break;

    /* ===== CLOCK ===== */
    case ACTION_CLOCK_STYLE_NEXT:
      dashData.clockStyle = (dashData.clockStyle + 1) % MAX_CLOCK_STYLES;
      preferences.begin("nmax_config", false);
      preferences.putInt("clk_style", dashData.clockStyle);
      preferences.end();
      break;

    /* ===== MENU ===== */
    case ACTION_MENU_ENTER:
      previousMode = currentMode;
      currentMode = MODE_MENU;
      menuIndex = 0;
      break;

    case ACTION_MENU_NEXT:
      menuIndex = (menuIndex + 1) % MENU_ITEM_COUNT;
      break;

    case ACTION_MENU_SELECT:
      if (menuIndex == 0) { // UI Style
        dashData.clockStyle = (dashData.clockStyle + 1) % MAX_CLOCK_STYLES;
        preferences.begin("nmax_config", false);
        preferences.putInt("clk_style", dashData.clockStyle);
        preferences.end();
      } else if (menuIndex == 1) { // Brightness
        dashData.brightness += 51; // ~20% steps
        if (dashData.brightness > 255) dashData.brightness = 51;
        dispMgr.setBrightness(dashData.brightness);
        preferences.begin("nmax_config", false);
        preferences.putInt("bri", dashData.brightness);
        preferences.end();
      } else if (menuIndex == 2) { // Reset Distance
        gpsMgr.resetDistance();
        dispMgr.renderToast("DIST RESET");
      } else if (menuIndex == 3) { // Exit
        currentMode = previousMode;
      }
      break;

    case ACTION_MENU_BACK:
      currentMode = previousMode;
      break;

    /* ===== WIFI / OTA ===== */
    case ACTION_OTA_MODE:
      previousMode = currentMode;
      currentMode = MODE_WIFI;
      dispMgr.renderToast("WIFI MODE");
      break;

    /* ===== TRIP RESET ===== */
    case ACTION_TRIP_RESET:
      tripMgr.reset(&dashData);
      dispMgr.renderToast("TIMER RESET");
      break;

    default:
      break;
  }
}

/* ================= HELPERS ================= */
void switchMode() {
  if (currentMode == MODE_CLOCK) currentMode = MODE_COMPASS;
  else if (currentMode == MODE_COMPASS) currentMode = MODE_ROBOT;
  else if (currentMode == MODE_ROBOT) {
    currentMode = MODE_GAME;
    gameState = GAME_READY;
  } else currentMode = MODE_CLOCK;

  dispMgr.renderToast(
    currentMode == MODE_COMPASS ? "COMPASS" :
    currentMode == MODE_ROBOT   ? "ROBOT" :
    currentMode == MODE_GAME    ? "GAME"  : "CLOCK"
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

  if (obstacleX < 35 && obstacleX > 10 && bikeY > 30) {
    gameState = GAME_OVER;
  }
}