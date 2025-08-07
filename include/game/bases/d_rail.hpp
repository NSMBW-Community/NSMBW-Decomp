#pragma once
#include <types.h>

class dRailInfo_c {
public:
    u8 mPad1;
    u8 mPad2;
    u16 mIdx;
    u16 m_04;
};

class dRail_c {
public:
    static dRailInfo_c *getRailInfoP(u16);
};
