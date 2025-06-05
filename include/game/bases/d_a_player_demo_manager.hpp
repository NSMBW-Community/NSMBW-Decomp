#pragma once
#include <types.h>

class daPyDemoMng_c {
public:
    void setCourseOutList(s8 playerNo);
    void clearDemoNo(s8 playerNo);
    bool checkDemoNo(s8 playerNo);
    void turnNextDemoNo();

    char mPad1[0x70];
    int m_70;
    char mPad2[0xc];
    int mPlNo;

    int getPlrNo() const { return mPlNo; }
    void setPlrNo(int playerNo) { mPlNo = playerNo; }

    static daPyDemoMng_c *mspInstance;
};
