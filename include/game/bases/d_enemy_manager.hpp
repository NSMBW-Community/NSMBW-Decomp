#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

class dEnemyMng_c {
public:
    void breakdownSE(int, const mVec3_c &);
    void incQuakeComboCount(int);

    u8 mPad1[0x138];
    int m_138;
    u8 mPad2[0x10];
    int m_14c;
    u8 mPad3[0x4];
    int m_154;

    static dEnemyMng_c *m_instance;
};
