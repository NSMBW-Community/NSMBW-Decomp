#pragma once

#include <types.h>

class dEnemyMng_c {
public:
    u8 mPad[0x138];
    int m_138;
    u8 mPad2[0x10];
    int m_14c;

    static dEnemyMng_c *m_instance;
};
