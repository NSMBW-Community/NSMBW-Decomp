#pragma once

#include <types.h>

class dPc_c {
public:
    dPc_c() : m_0c(0) {}
    virtual ~dPc_c() {}

    u8 mPad1[0x8];
    u32 m_0c;
    u8 mPad2[0x24];
};
