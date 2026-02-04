#ifndef GLOBALTYPES_H
#define GLOBALTYPES_H

#include <Arduino.h>

enum SystemMode {
    MODE_SPEEDO,
    MODE_NAVIGASI,
    MODE_COMPASS,
    MODE_MUSIC,
    MODE_NOTIFIKASI,
    MODE_GAME,
    MODE_ROBOT,
    MODE_FIND_BIKE,
    MODE_WIFI,
    MODE_MENU,
    MODE_CLOCK,

    MODE_ANY
};

enum RobotEmotion {
  ROBOT_NEUTRAL,
  ROBOT_HAPPY,
  ROBOT_LAUGH,
  ROBOT_SAD,
  ROBOT_CRY,
  ROBOT_ANGRY
};

// TAMBAHAN: Status Khusus Game
enum GameState {
    GAME_READY,   // Tampilan "PRESS TO START"
    GAME_PLAYING, // Game sedang jalan
    GAME_OVER     // Tampilan Score Akhir
};

struct NotificationData {
    String app;
    String sender;
    String message;
    unsigned long startTime;
    int iconType;
    bool isActive;
};

struct NavigationData {
    String distance;
    String unit;
    String action;
    String street;
    String eta;
    String appName;
    int iconType;
    bool isActive;
};

struct DashboardData {
    int currentSpeed;
    int maxSpeed;
    float tripDistance;
    unsigned long tripSeconds; // [NEW]
    float heading;
    int satellites;
    int batteryPhone;
    String currentTime;
    bool isGpsFixed;
    bool isBtConnected;
    String customName;
    int clockStyle;
    int brightness;
};

struct MusicData {
    String title;
    String artist;
    bool isPlaying;
};

struct OTAData {
    String ssid;
    String ip;
    String status;      
    int progress;       
    bool isUpdating;
};

struct DataContext {
    DashboardData    dash;
    NavigationData   nav;
    NotificationData notif;
    MusicData        music;
    OTAData          ota;

    SystemMode currentMode = MODE_SPEEDO;
    SystemMode previousMode = MODE_SPEEDO;
};

#endif