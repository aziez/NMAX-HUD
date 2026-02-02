#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "Config.h"
#include "GlobalTypes.h"

// Definisikan tipe fungsi callback untuk progress update
// (int persen, String status)
typedef std::function<void(int, String)> OtaCallback;

class NetworkManager {
  private:
    WebServer server;
    bool _isActive;
    unsigned long bootTime;
    
    // Variabel Callback disimpan di sini
    OtaCallback _progressCallback;

  public:
    NetworkManager();
    void begin();
    void handle();
    void startAP();
    void stopAP();
    bool isActive();
    void updateUI(OTAData* data);

    // Fungsi setter untuk menghubungkan callback
    void setProgressCallback(OtaCallback cb);
};

#endif