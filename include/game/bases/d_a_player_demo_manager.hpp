#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

class daPyDemoMng_c {
public:
    enum Mode_e {
        MODE_0,
        MODE_1
    };

    void setCourseOutList(s8 playerNo);
    void clearDemoNo(s8 playerNo);
    bool checkDemoNo(s8 playerNo);
    void turnNextDemoNo();
    int setGoalDemoList(int playerNo);
    void setDemoMode(daPyDemoMng_c::Mode_e, int);
    void stopBgmGoalDemo();
    int getPoleBelowPlayer(int playerNo);
    int getControlDemoPlayerNum() const;

    char mPad1[0x10];
    u32 mFlags;
    u32 m_14;
    char mPad2[0x4];
    int m_1c;
    char mPad3[0x22];
    bool m_42;
    char mPad4[0x2];
    mVec3_c mFireworkPos;
    char mPad5[0x8];
    int m_5c;
    char mPad6[0x10];
    int m_70;
    char mPad7[0xc];
    int mPlNo;
    u8 mPad8[0x10];
    int m_94;

    int getPlrNo() const { return mPlNo; }
    void setPlrNo(int playerNo) { mPlNo = playerNo; }

    static daPyDemoMng_c *mspInstance;
};
