#pragma once

class dfukidashiInfo_c {
public:
    bool mVisible;
    char pad[0x23b];

    static float
        c_MARIO_NORMAL_OFFSET_X,    c_MARIO_NORMAL_OFFSET_Y,
        c_MARIO_SUPER_OFFSET_X,     c_MARIO_SUPER_OFFSET_Y,
        c_MARIO_FIRE_OFFSET_X,      c_MARIO_FIRE_OFFSET_Y,
        c_MARIO_MAME_OFFSET_X,      c_MARIO_MAME_OFFSET_Y,
        c_MARIO_PROPEL_OFFSET_X,    c_MARIO_PROPEL_OFFSET_Y,
        c_MARIO_PENGUIN_OFFSET_X,   c_MARIO_PENGUIN_OFFSET_Y,
        c_MARIO_ICE_OFFSET_X,       c_MARIO_ICE_OFFSET_Y,
        c_LUIGI_NORMAL_OFFSET_X,    c_LUIGI_NORMAL_OFFSET_Y,
        c_LUIGI_SUPER_OFFSET_X,     c_LUIGI_SUPER_OFFSET_Y,
        c_LUIGI_FIRE_OFFSET_X,      c_LUIGI_FIRE_OFFSET_Y,
        c_LUIGI_MAME_OFFSET_X,      c_LUIGI_MAME_OFFSET_Y,
        c_LUIGI_PROPEL_OFFSET_X,    c_LUIGI_PROPEL_OFFSET_Y,
        c_LUIGI_PENGUIN_OFFSET_X,   c_LUIGI_PENGUIN_OFFSET_Y,
        c_LUIGI_ICE_OFFSET_X,       c_LUIGI_ICE_OFFSET_Y,
        c_KINOPIO_NORMAL_OFFSET_X,  c_KINOPIO_NORMAL_OFFSET_Y,
        c_KINOPIO_SUPER_OFFSET_X,   c_KINOPIO_SUPER_OFFSET_Y,
        c_KINOPIO_FIRE_OFFSET_X,    c_KINOPIO_FIRE_OFFSET_Y,
        c_KINOPIO_MAME_OFFSET_X,    c_KINOPIO_MAME_OFFSET_Y,
        c_KINOPIO_PROPEL_OFFSET_X,  c_KINOPIO_PROPEL_OFFSET_Y,
        c_KINOPIO_PENGUIN_OFFSET_X, c_KINOPIO_PENGUIN_OFFSET_Y,
        c_KINOPIO_ICE_OFFSET_X,     c_KINOPIO_ICE_OFFSET_Y;
};

class dfukidashiManager_c {
public:
    char pad[0x381];
    dfukidashiInfo_c mInfos[4];

    static dfukidashiManager_c *m_instance;
};
