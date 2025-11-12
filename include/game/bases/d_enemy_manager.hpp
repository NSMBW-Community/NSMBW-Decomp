#pragma once

#include <types.h>

class dEnemyMng_c {
public:
    u8 mPad[0x138];
    int m_138;

    static dEnemyMng_c *m_instance;
};
