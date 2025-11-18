#pragma once

#include <types.h>

class dWaterEntryMng_c {
public:
    u8 mPad[0x8c0];
    float m_8c0;

    float get_8c0() const { return m_8c0; }

    static dWaterEntryMng_c *m_instance;
};
