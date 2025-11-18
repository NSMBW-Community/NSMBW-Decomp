#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

class dEnemyMng_c {
public:
    void breakdownSE(int, const mVec3_c &);
    void incQuakeComboCount(int);
    void createRevivalBallon(mVec3_c &, int, int);
    void demo_ivy_create(mVec3_c *);

    u8 mPad1[0x138];
    int m_138;
    u8 mPad2[0x18];
    int m_154;
    u8 mPad3[0x4];
    int m_15c;

    static dEnemyMng_c *m_instance;
};
