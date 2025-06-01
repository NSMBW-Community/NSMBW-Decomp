#pragma once

#include <types.h>

class daPyDemoMng_c {
public:
    u8 mPad1[0x10];
    u32 mFlags;
    u8 mPad2[0x6c];
    int mPlNo;

    static daPyDemoMng_c *mspInstance;
};
