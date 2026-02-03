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

/* === SIMPLE BUTTON COMMAND === */
int InputManager::readButtonCommand() {
    static unsigned long lastPress = 0;

    if (digitalRead(PIN_TOMBOL) == LOW) {
        delay(20);
        if (digitalRead(PIN_TOMBOL) == LOW) {
            lastPress = millis();
            while (digitalRead(PIN_TOMBOL) == LOW);

            unsigned long dur = millis() - lastPress;
            if (dur > 800) return 99;
            return 1;
        }
    }
    return 0;
}
