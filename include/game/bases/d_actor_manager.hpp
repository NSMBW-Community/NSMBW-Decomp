#pragma once

#include <game/mLib/m_vec.hpp>
#include <game/bases/d_a_boss_demo.hpp>

class dActorMng_c {
public:
    u8 mPad1[0x18];
    daBossDemo_c *mpCurrentBossDemo;
    u8 mPad2[0xC];
    int mGoombaZOrderThing;
    u8 mPad3[0x218];

    void createUpCoin(const mVec3_c &pos, u8 dir, u8 count, u8 layer);
    void allEnemyDeath(int);

    static dActorMng_c *m_instance;
};
