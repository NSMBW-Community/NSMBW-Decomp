#pragma once
#include <types.h>

class SndSceneMgr {
public:
    void moveMissFin();
    void fn_8019d5b0(u8); ///< @unofficial
    void fn_8019bd90(int); ///< @unofficial
    void fn_8019be60(int); ///< @unofficial

    u8 mPad1[0x10];
    int m_10;
    int m_14;

    static SndSceneMgr *sInstance;
};
