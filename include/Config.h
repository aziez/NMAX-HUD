#ifndef CONFIG_H
#define CONFIG_H

// ================= HARDWARE =================
#define PIN_TOMBOL          4
#define GPS_RX_PIN          16
#define GPS_TX_PIN          17
// I2C Display default ESP32: SDA 21, SCL 22

// ================= WIFI & OTA =================
#define WIFI_SSID_PREFIX    "NMAX_HUD_"
#define WIFI_PASSWORD       "11223344"
#define WIFI_TIMEOUT        300000
#define OTA_PORT            80

// ================= BLUETOOTH =================
#define BT_DEVICE_NAME      "NMAX_HUD_PRO"

// ================= DISPLAY =================
#define DEFAULT_BRIGHTNESS  255
#define DRAW_INTERVAL       50      // 20 FPS

// ================= INPUT (BUTTON) =================
#define BTN_DEBOUNCE_MS     30
#define BTN_CLICK_MS        250
#define BTN_LONGPRESS_MS    3000
#define CLICK_TIMEOUT       500

// ================= TIMERS =================
#define NOTIF_DURATION      8000
#define MUSIC_TIMEOUT       10000
#define GPS_SYNC_INTERVAL   60000

// ================= DEFAULT =================
#define DEFAULT_NAME        "NMAX RIDER"

#endif
