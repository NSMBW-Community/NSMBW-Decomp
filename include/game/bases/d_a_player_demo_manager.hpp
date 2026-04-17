#pragma once

#include <game/bases/d_a_player_base.hpp>

class daPyDemoMng_c {
public:
    void setBossDownPlayerNo(int plNo);

    void setBossDown(daPlBase_c *player) {
        setBossDownPlayerNo(*player->getPlrNo());
    }

    static daPyDemoMng_c *getInstance() { return mspInstance; }

    char filler[0x80];
    int mPlNo;

    static daPyDemoMng_c *mspInstance;
};
