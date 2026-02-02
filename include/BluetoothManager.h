#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <ESP32Time.h>
#include "GlobalTypes.h"
#include "Config.h"

class BluetoothManager {
  private:
    BluetoothSerial SerialBT; // Ganti nama biar konsisten
    bool _isConnected; // Status

    Preferences* prefRef; 
    ESP32Time* rtcRef;    
    
    // Pointer ke mode sistem utama (agar bisa ganti mode dari BT)
    SystemMode* currentModePtr;
    SystemMode* previousModePtr;

    void processPacket(String packet, NavigationData* nav, NotificationData* notif, DashboardData* dash, MusicData* music, SystemMode* mode);

  public:
    BluetoothManager();
    
    void begin(const char* deviceName, Preferences* pref, ESP32Time* rtc); // const char* lebih aman
    void end();
    
    void bindMode(SystemMode* cur, SystemMode* prev);

    void update(NavigationData* nav, NotificationData* notif, DashboardData* dash, MusicData* music, SystemMode* mode);
    
    void sendDashboardUpdate(DashboardData data);
    bool isConnected(); // Getter
    void sendCommand(String cmd);
};

#endif