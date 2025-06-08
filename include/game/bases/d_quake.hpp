#pragma once
#include <types.h>

class dQuake_c {
public:
    enum TYPE_SHOCK_e {
        TYPE_0,
        TYPE_1,
        TYPE_2,
        TYPE_3,
        TYPE_4,
        TYPE_5,
        TYPE_6,
        TYPE_7,
        TYPE_8
    };

    void shockMotor(s8, TYPE_SHOCK_e, int, int);

    static dQuake_c *m_instance;
};
