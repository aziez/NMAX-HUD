#include "TripManager.h"

void TripManager::begin(DashboardData* data) {
    load(data);
    lastUpdateTime = millis();
    lastSaveTime = millis();
}

void TripManager::load(DashboardData* data) {
    prefs.begin("trip_data", true);
    data->tripSeconds = prefs.getUInt("seconds", 0);
    data->tripDistance = prefs.getFloat("dist", 0.0f);
    prefs.end();
}

void TripManager::update(DashboardData* data) {
    unsigned long now = millis();
    
    // Increment seconds if more than 1s elapsed
    if (now - lastUpdateTime >= 1000) {
        data->tripSeconds++;
        lastUpdateTime = now;
    }
    
    // Auto-save every 30 seconds to NVS
    if (now - lastSaveTime > 30000) {
        save(data->tripSeconds, data->tripDistance);
        lastSaveTime = now;
    }
}

void TripManager::save(unsigned long seconds, float distance) {
    prefs.begin("trip_data", false);
    prefs.putUInt("seconds", seconds);
    prefs.putFloat("dist", distance);
    prefs.end();
}

void TripManager::reset(DashboardData* data) {
    data->tripSeconds = 0;
    data->tripDistance = 0;
    save(0, 0);
}

String TripManager::getFormattedTime(unsigned long seconds) {
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;
    
    char buf[12];
    if (h > 0) sprintf(buf, "%d:%02d:%02d", h, m, s);
    else sprintf(buf, "%02d:%02d", m, s);
    return String(buf);
}
