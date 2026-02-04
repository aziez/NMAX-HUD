#include "InputManager.h"
#include "InputActionMap.h"
#include "Config.h"

void InputManager::begin() {
    pinMode(PIN_TOMBOL, INPUT_PULLUP);
}

void InputManager::bindMode(SystemMode* ptr) {
    modePtr = ptr;
}

void InputManager::update() {
    if (!modePtr) return;

    int cmd = readButtonCommand();
    if (cmd <= 0) return;

    for (int i = 0; i < INPUT_ACTION_MAP_SIZE; i++) {
        const InputMap& m = INPUT_ACTION_MAP[i];

        if ((m.mode == *modePtr || m.mode == MODE_ANY) &&
            m.command == cmd) {
            lastAction = m.action;
            return;
        }
    }
}

bool InputManager::hasAction() const {
    return lastAction != ACTION_NONE;
}

InputAction InputManager::consumeAction() {
    InputAction a = lastAction;
    lastAction = ACTION_NONE;
    return a;
}

int InputManager::readButtonCommand() {
    bool currentBtnState = digitalRead(PIN_TOMBOL);
    int result = 0;
    unsigned long now = millis();

    // Button Pressed
    if (currentBtnState == LOW && lastBtnState == HIGH) {
        btnPressStartTime = now;
        isLongPressHandled = false;
        isLongPress3sHandled = false;
        isLongPress4sHandled = false;
    }

    // Button Held (Tiered Long Press Detection)
    if (currentBtnState == LOW) {
        unsigned long holdTime = now - btnPressStartTime;
        
        if (holdTime > 4000 && !isLongPress4sHandled) {
            isLongPress4sHandled = true;
            isLongPress3sHandled = true;
            isLongPressHandled = true;
            clickCount = 0;
            result = 101; // Super Long Press (OTA)
        }
        else if (holdTime > 3000 && !isLongPress3sHandled) {
            isLongPress3sHandled = true;
            isLongPressHandled = true;
            clickCount = 0;
            result = 100; // Extra Long Press (Reset Trip)
        }
        else if (holdTime > 1000 && !isLongPressHandled) {
            isLongPressHandled = true;
            clickCount = 0;
            result = 99;  // Standard Long Press (Menu/Action)
        }
    }

    // Button Released
    if (currentBtnState == HIGH && lastBtnState == LOW) {
        if (!isLongPressHandled) {
            clickCount++;
            lastBtnReleaseTime = now;
        }
    }

    // Click Timeout (Multi-click detection)
    if (clickCount > 0 && (now - lastBtnReleaseTime > 300)) {
        result = clickCount;
        clickCount = 0;
    }

    lastBtnState = currentBtnState;
    return result;
}