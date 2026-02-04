#ifndef GPSMANAGER_H
#define GPSMANAGER_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include <ESP32Time.h>
#include "Config.h"
#include "GlobalTypes.h"

class GpsManager {
  private:
    TinyGPSPlus gps;
    HardwareSerial* gpsSerial;
    ESP32Time* rtcRef;

    double lastLat;
    double lastLon;
    float sessionDistance;
    float currentSpeedSmooth;
    int maxSpeed;

    unsigned long lastSyncTime;
    unsigned long lastGpsUpdate;

    void syncTime();

  public:
    GpsManager();
    void begin(ESP32Time* rtc);
    void update(DashboardData* data);
    float getDistance();
    void resetDistance();
};

#endif
