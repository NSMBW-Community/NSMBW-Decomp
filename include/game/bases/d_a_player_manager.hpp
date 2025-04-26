#pragma once
#include <game/bases/d_a_player.hpp>

class daPyMng_c {
public:
    static dAcPy_c *getPlayer(int);
    static int mPlayerType[4];
    static int mPlayerMode[4];
};
