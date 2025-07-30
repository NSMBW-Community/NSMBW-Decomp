#pragma once
#include <game/bases/d_mj2d_data.hpp>

class dSaveMng_c {
public:
    dMj2dGame_c *getSaveGame(s8);

    static dSaveMng_c *m_instance;
};
