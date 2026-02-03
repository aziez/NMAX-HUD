#include "InputActionMap.h"

/*
 command:
 1 = klik
 2 = double
 3 = triple
 99 = long press
*/

const InputMap INPUT_ACTION_MAP[] = {

    // ===== GLOBAL =====
    { MODE_ANY,   99, ACTION_OTA_MODE },

    // ===== NORMAL UI =====
    { MODE_SPEEDO, 1, ACTION_NEXT_SCREEN },
    { MODE_COMPASS,1, ACTION_NEXT_SCREEN },
    { MODE_ROBOT,  1, ACTION_NEXT_SCREEN },

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
