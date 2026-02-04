#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "GlobalTypes.h"
#include "InputAction.h"

class InputManager {
public:
    void begin();
    void update();

    void bindMode(SystemMode* modePtr);

    bool hasAction() const;
    InputAction consumeAction();

private:
    SystemMode* modePtr = nullptr;
    InputAction lastAction = ACTION_NONE;

    int readButtonCommand();
    
    // Non-blocking button state
    bool lastBtnState = HIGH;
    unsigned long btnPressStartTime = 0;
    unsigned long lastBtnReleaseTime = 0;
    int clickCount = 0;
    
    // Tiered Long Press Flags
    bool isLongPressHandled = false;
    bool isLongPress3sHandled = false;
    bool isLongPress4sHandled = false;
};

#endif