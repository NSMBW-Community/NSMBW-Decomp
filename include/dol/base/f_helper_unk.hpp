#pragma once

// Unofficial name
class fBaHelper_c {
    short mStatus;
    short mCount;
    void **mpArr;
    void *filler1;
    short filler2;
    short mUnknown;

public:
    virtual void vf_0x8(); // stripped out of binary
    virtual int vf_0xc(void *); // stripped out of binary
    virtual void vf_0x10(void *); // stripped out of binary

    void Delete();
    int Load(int);
    bool LoadOnlyOne();
};