#pragma once
#include <lib/egg/heap/eggHeap.hpp>

class dPanelObjMgr_c {
    u8 mPad[12];
};

class bgTex_c {
public:
    bgTex_c();
    virtual ~bgTex_c() {}
    void create(u16, u16, u16, u16, int, int, EGG::Heap *, u16);
    void setOpaBuffer(u16, u16, u16);
    void setUnitChange(u16, u16, u16);

    u16 mCount;
    u16 mSmth;
    u8 mPad[0x28];
    u16 m_30;
    u8 mPad2[6];
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
