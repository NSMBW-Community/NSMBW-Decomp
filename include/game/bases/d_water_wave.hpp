#pragma once

#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/mLib/m_vec.hpp>
#include <nw4r/g3d.h>

namespace d3d {

class rtexQuadManyTwin_c : public m3d::proc_c {
public:
    bool create(int, nw4r::g3d::ResTex, nw4r::g3d::ResTex, int, int, mAllocator_c *);
    void setOfs(mVec3_c *, int);

    u8 mPad1[0x44];
    mVec3_c mVertices[4][1100];
    int mCount;
    GXColor mColor1;
    GXColor mColor2;
    u8 mPad2[0x8];
};

class rtexMagaDeco_c : public m3d::proc_c {
public:
    bool create(nw4r::g3d::ResTex, int, int, mAllocator_c *);
    void setTexObj(int, nw4r::g3d::ResTex);

    GXTexObj mTexObjs[12];
    mVec3_c mVertices[20];
    float m_278[20];
    u8 m_2c8[20];
    u8 mObjIndices[20];
    GXColor m_2f0[20];
    GXColor m_340[20];
};

}

struct dWaterWave_sub {
    int m_00[200];
    float m_320[200];
    float m_640;
    float m_644[200];
    int m_964;
    int m_968;
    u8 m_96c[200];
    u8 m_a34;
};

class dWaterWave_c {
public:
    void wave_tex_set(mVec3_c *pos, float f, int i, u8 a);
    void waveDecoExecute();
    void waveExecute();
    void draw();
    void doDelete();

    dHeapAllocator_c mAllocator1;
    dHeapAllocator_c mAllocator2;
    nw4r::g3d::ResFile mRes1;
    nw4r::g3d::ResFile mRes2;
    d3d::rtexQuadManyTwin_c mWaveQuad;
    d3d::rtexMagaDeco_c mWaveDeco;
    mVec3_c mPos;
    float m_d284;
    mMtx_c mMtx;
    int m_d2e8;
    int m_d2fc;
    u8 m_d308;
    u8 m_d30a;
    dWaterWave_sub mSub[20];
};
