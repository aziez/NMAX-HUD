#include "GpsManager.h"

GpsManager::GpsManager() {
    gpsSerial = &Serial2;
    lastLat = 0.0;
    lastLon = 0.0;
    sessionDistance = 0.0;
    currentSpeedSmooth = 0;
    maxSpeed = 0;
    lastSyncTime = 0;
    lastGpsUpdate = 0;
}

void GpsManager::begin(ESP32Time* rtc) {
    rtcRef = rtc;

    // Use 9600 for NEO-6M
    gpsSerial->begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    gpsSerial->setRxBufferSize(2048);

    Serial.println("GPS NEO-6M Init...");

    delay(500);

    // Filter NMEA Sentences to reduce ESP32 load
    // Disable GLL, VTG, ZDA, GSV (keep RMC, GGA, GSA for lock & sat info)
    const char* cmds[] = {
        "$PUBX,40,GLL,0,0,0,0,0,0*5C",
        "$PUBX,40,VTG,0,0,0,0,0,0*5E",
        "$PUBX,40,ZDA,0,0,0,0,0,0*44"
    };

    for(int i=0; i<3; i++) {
        gpsSerial->println(cmds[i]);
        delay(100);
    }
    
    Serial.println("GPS Optimized.");
}

void GpsManager::update(DashboardData* data) {

    // === 1. BACA SEMUA DATA GPS (AGRESIF) ===
    // Jangan biarkan buffer penuh
    while (gpsSerial->available() > 0) {
        gps.encode(gpsSerial->read());
    }

    // === 2. CEK DATA FRESH ===
    if (gps.location.isUpdated()) {
        lastGpsUpdate = millis();
    }

    bool isFresh = (millis() - lastGpsUpdate) < 3000;

    // === 3. FIX STATUS (REAL FIX) ===
    bool isFixed =
        gps.location.isValid() &&
        gps.location.age() < 5000 &&
        gps.satellites.isValid() &&
        gps.satellites.value() >= 4 &&
        gps.hdop.isValid() &&
        gps.hdop.value() < 400; // <4.0 (penting buat ketutup body)

    data->isGpsFixed = isFixed;
    data->satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;

    // === 4. SPEED (ANTI NGACO) ===
    if (isFixed && gps.speed.isValid()) {
        int rawSpeed = gps.speed.kmph();

        // Anti lonjakan
        if (abs(rawSpeed - currentSpeedSmooth) < 30) {
            currentSpeedSmooth = currentSpeedSmooth * 0.8 + rawSpeed * 0.2;
        }

        if ((int)currentSpeedSmooth > maxSpeed)
            maxSpeed = (int)currentSpeedSmooth;
    } else {
        // Saat hilang sinyal → turunin speed pelan
        currentSpeedSmooth *= 0.9;
        if (currentSpeedSmooth < 1) currentSpeedSmooth = 0;
    }

    // === 5. ODOMETER (ANTI DRIFT TOTAL) ===
    if (isFixed && gps.speed.kmph() > 3.0) {
        double lat = gps.location.lat();
        double lon = gps.location.lng();

        if (lastLat != 0.0 && lastLon != 0.0) {
            double dist = TinyGPSPlus::distanceBetween(
                lat, lon, lastLat, lastLon
            );

            // Filter keras (motor getar + ketutup)
            if (dist > 5.0 && dist < 80.0) {
                sessionDistance += dist;
            }
        }

        lastLat = lat;
        lastLon = lon;
    }

    // === 6. HEADING ===
    if (isFixed && gps.course.isValid()) {
        data->heading = gps.course.deg();
    }

    // === 7. TIME SYNC (HANYA SAAT FIX) ===
    if (isFixed && millis() - lastSyncTime > GPS_SYNC_INTERVAL) {
        syncTime();
    }

    // === 8. OUTPUT ===
    data->currentSpeed = (int)currentSpeedSmooth;
    data->maxSpeed     = maxSpeed;
    data->tripDistance = sessionDistance;
}

void GpsManager::syncTime() {
    if (!gps.time.isValid() || !gps.date.isValid()) return;

    int hour = gps.time.hour() + 7;
    int day  = gps.date.day();

    if (hour >= 24) {
        hour -= 24;
        day += 1;
    }

    rtcRef->setTime(
        gps.time.second(),
        gps.time.minute(),
        hour,
        day,
        gps.date.month(),
        gps.date.year()
    );

    lastSyncTime = millis();
}

float GpsManager::getDistance() {
    return sessionDistance;
}

void GpsManager::resetDistance() {
    sessionDistance = 0;
    maxSpeed = 0;
}
