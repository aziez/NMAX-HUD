#include "InputActionMap.h"

/*
 command:
 1 = klik
 2 = double
 3 = triple
 99 = long press
*/

const InputMap INPUT_ACTION_MAP[] = {

    // ===== CLOCK STYLE =====
    { MODE_CLOCK,  1, ACTION_CLOCK_STYLE_NEXT },
    { MODE_CLOCK,  99, ACTION_MENU_ENTER },
    { MODE_ANY,    101, ACTION_OTA_MODE }, // 4s Hold
    { MODE_CLOCK,  100, ACTION_TRIP_RESET }, // 3s Hold in Clock
    { MODE_SPEEDO, 100, ACTION_TRIP_RESET }, // 3s Hold in Speedo

    // ===== MENU =====
    { MODE_MENU,   1, ACTION_MENU_NEXT },
    { MODE_MENU,   2, ACTION_MENU_SELECT },
    { MODE_MENU,   99, ACTION_MENU_BACK },

    // ===== NORMAL UI =====
    { MODE_SPEEDO, 1, ACTION_NEXT_SCREEN },
    { MODE_COMPASS,1, ACTION_NEXT_SCREEN },
    { MODE_ROBOT,  1, ACTION_NEXT_SCREEN },
    { MODE_CLOCK,  2, ACTION_NEXT_SCREEN },

    // ===== MUSIC =====
    { MODE_MUSIC, 1, ACTION_MUSIC_NEXT },
    { MODE_MUSIC, 2, ACTION_MUSIC_PLAY_PAUSE },
    { MODE_MUSIC, 3, ACTION_MUSIC_PREV },

    // ===== GAME =====
    { MODE_GAME,  1, ACTION_GAME_JUMP },
    { MODE_GAME,  99, ACTION_GAME_EXIT }
};

const int INPUT_ACTION_MAP_SIZE =
    sizeof(INPUT_ACTION_MAP) / sizeof(INPUT_ACTION_MAP[0]);
