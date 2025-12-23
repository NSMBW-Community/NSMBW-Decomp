#pragma once

#include <types.h>
#include <game/mLib/m_3d.hpp>
#include <lib/egg/core/eggHeap.h>

class dPanelObjMgr_c {
    u8 mPad[12];
};

class bgTex_c : public m3d::proc_c {
public:
    bgTex_c();
    virtual ~bgTex_c() {}
    void create(u16, u16, u16, u16, int, int, EGG::Heap *, u16);
    void setOpaBuffer(u16, u16, u16);
    void setUnitChange(u16, u16, u16);

    mAllocator_c mAllocator;
    u16 m_24;
    u16 m_26;
    void *m_28;
    void *m_2c;
    u16 mXCount;
    u16 mYCount;
    u8 mPad[4];
    dPanelObjMgr_c mPanelObjMgr;
};

class dBgTexMng_c {
public:
    dBgTexMng_c();
    virtual ~dBgTexMng_c();

    void create(EGG::Heap *);

    u8 mPad[0x54];
    bool mWmSwitchRelated;
};
