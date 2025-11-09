#pragma once

#include <game/bases/d_actor.hpp>

class dIceEfScale_c {
public:
    dIceEfScale_c() {
        mData[0] = 0.0f;
        mData[1] = 0.0f;
        mData[2] = 0.0f;
        mData[3] = 0.0f;
        mData[4] = 0.0f;
        mData[5] = 0.0f;
        mData[6] = 0.0f;
        mData[7] = 0.0f;
    }

    float mData[8];
};

class dIceInfo {
public:
    ~dIceInfo() {}

    int mMode;
    mVec3_c mPos;
    mVec3_c mSize;
    dIceEfScale_c mEfScale;
};

class dIceMng_c {
public:
    enum PROC_e {
        PROC_FROZEN,
        PROC_MELT,
        PROC_2,
        PROC_DEFAULT
    };

    dIceMng_c(dActor_c *owner);
    ~dIceMng_c();

    void initialize();
    PROC_e manageProc();
    void breakEffect();
    void removeIce();
    bool checkInstantBreak(int);
    bool createIce(dIceInfo *info, int count);

    u8 mPad1[0xc];
    int mActive;
    u8 mPad2[0x8];
    int mDestroyMode;
    u8 mPad3[0x14];
    int mPlrNo;
    u8 mPad4[0x38];
};
