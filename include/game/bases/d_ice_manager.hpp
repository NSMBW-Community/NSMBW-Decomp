#pragma once

#include <game/bases/d_actor.hpp>
#include <game/bases/d_ice_effect_scale.hpp>

class dIceInfo {
public:
    ~dIceInfo();

    int mMode;
    mVec3_c mPos;
    mVec3_c mSize;
    dIceEfScale_c mEfScale;
};

class dIceMng_c {
public:
    /// @unofficial
    enum PROC_e {
        PROC_FROZEN,
        PROC_MELT,
        PROC_2,
        PROC_DEFAULT
    };

    /// @unofficial
    enum DESTROY_MODE_e {
        DESTROY_NONE,
        DESTROY_BREAK,
        DESTROY_VANISH = 3
    };

    dIceMng_c(dActor_c *owner);
    ~dIceMng_c();

    void initialize();
    PROC_e manageProc();
    void breakEffect();
    void removeIce();
    bool checkInstantBreak(int);
    bool createIce(dIceInfo *info, int count);
    void setNonMeltTime();
    void stopMeltShake();

    void setDestroyMode(DESTROY_MODE_e mode) { mDestroyMode = mode; }
    void set_14(int value) { m_14 = value; }
    void set_2c(int value) { m_2c = value; }

    u8 mPad1[0x4];
    int m_04;
    int m_08;
    int mActive;
    u8 mPad3[0x4];
    int m_14;
    DESTROY_MODE_e mDestroyMode;
    u8 mPad4[0x10];
    int m_2c;
    int mPlrNo;
    u8 mPad5[0x38];
};
