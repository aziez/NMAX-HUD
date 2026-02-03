#ifndef INPUT_ACTION_MAP_H
#define INPUT_ACTION_MAP_H

#include "GlobalTypes.h"
#include "InputAction.h"

struct InputMap {
    SystemMode mode;
    int command;
    InputAction action;
};

extern const InputMap INPUT_ACTION_MAP[];
extern const int INPUT_ACTION_MAP_SIZE;

#endif
