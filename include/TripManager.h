#ifndef TRIP_MANAGER_H
#define TRIP_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "GlobalTypes.h"

class TripManager {
public:
    void begin(DashboardData* data);
    void update(DashboardData* data);
    void reset(DashboardData* data);
    
    static String getFormattedTime(unsigned long seconds);

private:
    unsigned long lastUpdateTime = 0;
    unsigned long lastSaveTime = 0;
    Preferences prefs;
    
    void load(DashboardData* data);
    void save(unsigned long seconds, float distance);
};

#endif
