#pragma once
#include <types.h>

class SndSceneMgr {
public:
    u8 mPad1[0x10];
    int m_10;
    int m_14;

    static SndSceneMgr *sInstance;
};
