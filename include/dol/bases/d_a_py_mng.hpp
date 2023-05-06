#pragma once
#include <dol/bases/d_base_actor.hpp>

class daPyMng_c {
public:
    static dBaseActor_c *getPlayer(int);
    static int mPlayerType[4];
    static int mPlayerMode[4];
};
