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
    void setOfs(mVec3_c *, int, int);

    u8 mPad1[0x40];
    mVec3_c mQuads[4400];
    nw4r::ut::Color mColor1;
    nw4r::ut::Color mColor2;
    int mCount;
    u8 mPad2[0x4];
    mAng mAngle1;
    mAng mAngle2;

    mVec3_c *getQuad(int i) { return &mQuads[i]; }
};

class rtexMagaDeco_c : public m3d::proc_c {
public:
    bool create(nw4r::g3d::ResTex, int, int, mAllocator_c *);
    void setTexObj(int, nw4r::g3d::ResTex);

    GXTexObj mTexObjs[12];
    mVec3_c mVertices[20];
    float m_278[20];
    u8 mState[20];
    u8 mObjIndices[20];
    nw4r::ut::Color mColor1[20];
    nw4r::ut::Color mColor2[20];

    mVec3_c *getVertex(int i) { return &mVertices[i]; }
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

    u8 get_a34() const { return m_a34; }
};

class dWaterWave_c {
public:
    void wave_tex_set(mVec3_c *pos, float f, int i, u8 a);
    void waveDecoExecute();
    void waveExecute();
    void draw();
    void doDelete();

    struct sSomeStruct {
        float f1;
        short s1;
        short s2;
        float f2;
        short s3;
        short s4;
    };

    struct GlobalData_t {
        GXColor mColors[12];
        float mFloats[12];
        sSomeStruct mStructs[3];
        float mFloats2[139];
        GXColor mWaterTexColor1;
        GXColor mWaterTexColor2;
    };

    dHeapAllocator_c mAllocatorWave;
    dHeapAllocator_c mAllocatorDeco;
    nw4r::g3d::ResFile mResWave;
    nw4r::g3d::ResFile mResDeco;
    d3d::rtexQuadManyTwin_c mWaveQuad;
    d3d::rtexMagaDeco_c mWaveDeco;
    mVec3_c mPos;
    mVec3_c mPos2;
    float m_d284;
    mMtx_c mMtxWave;
    mMtx_c mMtxDeco;
    int m_d2e8;
    int m_d2ec;
    int m_d2f0;
    u8 mPad3[0x4];
    int m_d2f8;
    int m_d2fc;
    int m_d300;
    int m_d304;
    u8 m_d308;
    u8 m_d309;
    u8 m_d30a;
    dWaterWave_sub mSub[20];

    dWaterWave_sub *getSub(int i) { return &mSub[i]; }
};
