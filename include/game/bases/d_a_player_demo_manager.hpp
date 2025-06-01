#pragma once

class daPyDemoMng_c {
public:
    char filler1[0x10];
    u32 mFlags;
    char filler2[0x6c];
    int mPlNo;

    static daPyDemoMng_c *mspInstance;
};
