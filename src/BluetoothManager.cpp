#include "BluetoothManager.h"

BluetoothManager::BluetoothManager() {
    _isConnected = false;
    currentModePtr = nullptr;
    previousModePtr = nullptr;
}

void BluetoothManager::begin(const char* deviceName, Preferences* prefs, ESP32Time* rtcRef) {
    this->prefRef = prefs;
    this->rtcRef = rtcRef;

    if (!SerialBT.begin(deviceName)) {
        Serial.println("[BT] Failed to start!");
    } else {
        Serial.println("[BT] Started: " + String(deviceName));
    }
}

void BluetoothManager::end() {
    if (SerialBT.hasClient()) {
        SerialBT.disconnect();
    }
    SerialBT.end();
    _isConnected = false;
    Serial.println("[BT] Stopped");
}

void BluetoothManager::bindMode(SystemMode* cur, SystemMode* prev) {
    this->currentModePtr = cur;
    this->previousModePtr = prev;
}

void BluetoothManager::update(NavigationData* nav, NotificationData* notif, DashboardData* dash, MusicData* music, SystemMode* mode) {
    if (!SerialBT.hasClient()) {
        _isConnected = false;
        dash->isBtConnected = false;
        dash->batteryPhone = -1;
        return;
    }

    _isConnected = true;
    dash->isBtConnected = true;

    if (SerialBT.available()) {
        String payload = SerialBT.readStringUntil('\n');
        payload.trim(); 
        if (payload.length() > 0) {
            processPacket(payload, nav, notif, dash, music, mode);
        }
    }
}

void BluetoothManager::sendCommand(String cmd) {
    if (SerialBT.hasClient()) {
        SerialBT.println(cmd);
    }
}

void BluetoothManager::processPacket(String packet, NavigationData* nav, NotificationData* notif, DashboardData* dash, MusicData* music, SystemMode* mode) {
    
    // 1. SETTINGS COMMANDS (Prefix "SET:")
    if (packet.startsWith("SET:")) {
        
        // --- SET NAME ---
        if (packet.startsWith("SET:NAME:")) {
            String newName = packet.substring(9);
            if (newName.length() > 0) {
                dash->customName = newName;
                
                // [FIX PERSISTENCE]
                prefRef->begin("nmax_config", false);
                prefRef->putString("name", newName);
                prefRef->end();
                
                Serial.println("[BT] Name Updated & Saved: " + newName);
            }
            return;
        }

        // --- SET TIME ---
        if (packet.startsWith("SET:TIME:")) {
            String timeVal = packet.substring(9); 
            int separatorIndex = timeVal.indexOf(':');
            if (separatorIndex > 0) {
                int h = timeVal.substring(0, separatorIndex).toInt();
                int m = timeVal.substring(separatorIndex + 1).toInt();
                rtcRef->setTime(0, m, h, 1, 1, 2024);
                Serial.printf("[BT] Time Synced: %02d:%02d\n", h, m);
            }
            return;
        }

        // --- SET BRIGHTNESS ---
        if (packet.startsWith("SET:BRI:")) {
            int brightness = packet.substring(8).toInt();
            
            prefRef->begin("nmax_config", false);
            prefRef->putInt("bri", brightness);
            prefRef->end();

            Serial.printf("[BT] Brightness Saved: %d\n", brightness);
            return;
        }
    }
    
    // 2. MODE CHANGE
    if (packet.startsWith("MODE:")) {
        String target = packet.substring(5); target.toUpperCase();
        if (currentModePtr) {
            if (target == "SPEEDO") *currentModePtr = MODE_SPEEDO;
            else if (target == "NAV") *currentModePtr = MODE_NAVIGASI;
            else if (target == "COMPASS") *currentModePtr = MODE_COMPASS;
            else if (target == "MUSIC") *currentModePtr = MODE_MUSIC;
            else if (target == "GAME") *currentModePtr = MODE_GAME;
            else if (target == "ROBOT") *currentModePtr = MODE_ROBOT;
            else if (target == "WIFI") *currentModePtr = MODE_WIFI;
            else if (target == "NOTIF") *currentModePtr = MODE_NOTIFIKASI;
        }
        return; 
    }

    // 3. SPECIAL COMMANDS
    if (packet.startsWith("CMD:")) {
        String cmd = packet.substring(4);
        if (cmd == "FIND") {
            if (currentModePtr) *currentModePtr = MODE_FIND_BIKE;
        }
        return;
    }

    // 4. JSON DATA (Nav, Notif, dll)
    JsonDocument doc; 
    DeserializationError error = deserializeJson(doc, packet);
    if (error) return; 

    const char* modeStr = doc["mode"];
    if (!modeStr) return; 
    String typeStr = String(modeStr);

    if (typeStr == "NAV") {
        int iconId = doc["icon"].as<int>();
        switch(iconId) {
            case 1: nav->action = "turn_left"; break;
            case 2: nav->action = "turn_right"; break;
            case 3: nav->action = "round_left"; break;
            case 4: nav->action = "round_right"; break;
            case 5: nav->action = "flag"; break;
            case 6: nav->action = "u_turn"; break;
            case 8: nav->action = "straight"; break;
            default: nav->action = "straight"; break;
        }
        nav->distance = doc["dist"].as<String>() + " " + doc["unit"].as<String>();
        nav->street   = doc["str"].as<String>();
        nav->appName  = doc["app"].as<String>();
        nav->eta      = doc["eta"].as<String>(); 
        nav->isActive = true;
        if (currentModePtr && *currentModePtr == MODE_SPEEDO) *currentModePtr = MODE_NAVIGASI;
    }
    else if (typeStr == "NOTIF") {
        notif->app = doc["app"].as<String>();
        notif->sender = doc["title"].as<String>();
        notif->message = doc["msg"].as<String>();
        notif->isActive = true; notif->startTime = millis();
        if (currentModePtr && *currentModePtr != MODE_WIFI && *currentModePtr != MODE_GAME) *currentModePtr = MODE_NOTIFIKASI;
    }
    else if (typeStr == "MUSIC") {
        String fullTitle = doc["title"].as<String>();
        int dashIndex = fullTitle.indexOf(" - ");
        if (dashIndex > 0) {
            music->title = fullTitle.substring(0, dashIndex);
            music->artist = fullTitle.substring(dashIndex + 3);
        } else {
            music->title = fullTitle;
            music->artist = ""; 
        }
        music->isPlaying = true;
        if (currentModePtr && *currentModePtr == MODE_SPEEDO) *currentModePtr = MODE_MUSIC;
    }
    else if (typeStr == "BAT") {
        dash->batteryPhone = doc["val"].as<int>();
    }
}

bool BluetoothManager::isConnected() {
    return _isConnected; 
}

void BluetoothManager::sendDashboardUpdate(DashboardData data) {
    if (!_isConnected) return;
}