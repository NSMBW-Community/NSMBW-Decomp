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

    u8 mPad1[0x40];
    mVec3_c mQuads[4][1100];
    nw4r::ut::Color mColor1;
    nw4r::ut::Color mColor2;
    int mCount;
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
    nw4r::ut::Color m_2f0[20];
    nw4r::ut::Color m_340[20];
};

}

struct dWaterWave_sub {
    float m_00[200];
    float m_320[200];
    float m_640;
    int m_644[200];
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

    struct GlobalData_t {
        GXColor mColors[12];
        float mFloats[12];
        struct {
            float f;
            short s1, s2;
        } mStructs[6];
        float mFloats2[139];
        GXColor mWaterTexColor1;
        GXColor mWaterTexColor2;
    };

    dHeapAllocator_c mAllocator1;
    dHeapAllocator_c mAllocator2;
    nw4r::g3d::ResFile mRes1;
    nw4r::g3d::ResFile mRes2;
    d3d::rtexQuadManyTwin_c mWaveQuad;
    d3d::rtexMagaDeco_c mWaveDeco;
    mVec3_c mPos;
    u8 mPad1[0xc];
    float m_d284;
    mMtx_c mMtxWave;
    mMtx_c mMtxDeco;
    int m_d2e8;
    u8 mPad3[0x10];
    int m_d2fc;
    u8 mPad4[0x8];
    u8 m_d308;
    u8 m_d309;
    u8 m_d30a;
    dWaterWave_sub mSub[20];
};
