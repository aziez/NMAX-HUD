#ifndef CONFIG_H
#define CONFIG_H

// --- Hardware Pins ---
#define PIN_TOMBOL      4
#define GPS_RX_PIN      16
#define GPS_TX_PIN      17
// I2C Display menggunakan Default ESP32 (SDA 21, SCL 22)

// --- Settings WiFi & OTA ---
#define WIFI_SSID_PREFIX "NMAX_HUD_"
#define WIFI_PASSWORD    "11223344"
#define WIFI_TIMEOUT     300000      
#define OTA_PORT         80

// --- Settings Bluetooth ---
#define BT_DEVICE_NAME   "NMAX_HUD_PRO"

// --- Timers & Thresholds ---
#define NOTIF_DURATION   8000
#define DRAW_INTERVAL    50   // 20 FPS (1000ms / 50ms)
#define MUSIC_TIMEOUT    10000
#define GPS_SYNC_INTERVAL 60000 // Sync jam tiap 1 menit

// --- Default Values ---
#define DEFAULT_NAME     "NMAX RIDER"
#define DEFAULT_BRIGHTNESS 255

#endif