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
};

#endif
