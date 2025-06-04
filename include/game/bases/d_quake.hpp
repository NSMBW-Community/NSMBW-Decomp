#pragma once
#include <types.h>

class dQuake_c {
public:
    void shockMotor(s8, int, int, int); // actually TYPE_SHOCK_e

    static dQuake_c *m_instance;
};
