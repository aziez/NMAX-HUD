#include "InputManager.h"

InputManager::InputManager() {
    btn = nullptr; 
    clickCount = 0;
    lastClickTime = 0;
    finalCommand = 0;
    jumpTriggered = false;
    modePtr = nullptr;
    
    // Manual debounce vars
    lastSteadyState = HIGH;
    lastFlickerableState = HIGH;
    lastDebounceTime = 0;
    btnPressTime = 0; // Init
}

void InputManager::begin() {
    // 1. HARDWARE INIT
    pinMode(PIN_TOMBOL, INPUT_PULLUP);

    // 2. LIBRARY INIT
    btn = new OneButton(PIN_TOMBOL, true, true);
    
    btn->setDebounceMs(30);
    
    // Agar responsif untuk hitungan manual (1x, 2x, 3x)
    btn->setClickMs(20); 

    // [UBAH] 5 Detik terlalu lama, ganti jadi 3 Detik (3000ms)
    btn->setPressMs(3000); 

    btn->attachClick(staticClick, this);
    btn->attachLongPressStart(staticLongPress, this);
    
    Serial.println("InputManager Started on Pin " + String(PIN_TOMBOL));
}

void InputManager::bindMode(SystemMode *mode) {
    modePtr = mode;
}

void InputManager::update() {
    // 1. UPDATE LIBRARY
    if (btn) btn->tick();

    // 2. MANUAL READ GLOBAL (Berlaku untuk SEMUA mode)
    // Kita baca ini untuk:
    // A. Deteksi Game Jump (Instant)
    // B. Mencatat waktu tekan (btnPressTime) untuk filter "Klik Palsu"
    
    int currentState = digitalRead(PIN_TOMBOL);

    if (currentState != lastFlickerableState) {
        lastDebounceTime = millis();
        lastFlickerableState = currentState;
    }

    if ((millis() - lastDebounceTime) > 20) {
        // Jika status stabil berubah
        if (lastSteadyState != currentState) {
            lastSteadyState = currentState;

            // DETEKSI DITEKAN (Falling Edge)
            if (lastSteadyState == LOW) {
                // Catat waktu mulai tekan
                btnPressTime = millis();

                // Khusus Mode Game: Trigger Jump
                if (modePtr && *modePtr == MODE_GAME) {
                    jumpTriggered = true;
                }
            }
        }
    }

    // 3. LOGIC MULTI-CLICK
    if (clickCount > 0 && (millis() - lastClickTime > CLICK_TIMEOUT)) {

    if (modePtr) {

        // 🔒 MODE MUSIC: hanya 1–3 klik, TANPA UI SWITCH
        if (*modePtr == MODE_MUSIC) {
            if (clickCount <= 3)
                finalCommand = clickCount;
        }

        else if (*modePtr == MODE_GAME) {
            if (clickCount >= 4)
                finalCommand = 4;
        }

        else if (*modePtr != MODE_WIFI) {
            finalCommand = clickCount;
        }
    }

        clickCount = 0;
    }
}

// ---------------- CALLBACKS ----------------

void InputManager::onClick() {
    if (modePtr && *modePtr == MODE_WIFI) return;

    unsigned long pressDuration = millis() - btnPressTime;

    // Abaikan klik "ragu-ragu"
    if (pressDuration > 800) return;

    // 🔒 MODE MUSIC: max 3 klik
    if (modePtr && *modePtr == MODE_MUSIC && clickCount >= 3)
        return;

    clickCount++;
    lastClickTime = millis();
}
void InputManager::onLongPress() {
    // Dipanggil setelah tahan 3 Detik
    finalCommand = 99; // Masuk OTA
}

// ---------------- STATIC WRAPPERS ----------------

void InputManager::staticClick(void *ptr) { ((InputManager*)ptr)->onClick(); }
void InputManager::staticLongPress(void *ptr) { ((InputManager*)ptr)->onLongPress(); }

// ---------------- GETTERS ----------------

int InputManager::getCommand() {
    if (finalCommand > 0) {
        int c = finalCommand; finalCommand = 0; return c;
    }
    return 0;
}

bool InputManager::isGameJump() {
    if (jumpTriggered) { jumpTriggered = false; return true; }
    return false;
}