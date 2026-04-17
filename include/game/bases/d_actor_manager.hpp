#pragma once

#include <game/mLib/m_vec.hpp>

class dActorMng_c {
public:
    u8 mPad1[0x28];
    int mGoombaZOrderThing;
    u8 mPad2[0x218];

    void createUpCoin(const mVec3_c &pos, u8 dir, u8 count, u8 layer);
    void allEnemyDeath(int);

    static dActorMng_c *m_instance;
};
