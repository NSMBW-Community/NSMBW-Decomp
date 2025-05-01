#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_3d.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_bg_global.hpp>
#include <game/bases/d_bg_tex.hpp>
#include <game/bases/d_cd.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_3d.hpp>

struct dBgCheckUnit {
    // dBgCheckUnit() {}
    // float x, y;
    unsigned long long a;
    // u8 a, b, c;
    // int d;
    // u8 test[8];
    // u32 flags[2];

    // bool checkBit(int bit) {
    //     if (bit > 32) {
    //         return flags[1] & (1 << (bit - 32));
    //     } else {
    //         return flags[0] & (1 << bit);
    //     }
    // }
    // void setBit(int bit) {
    //     if (bit > 32) {
    //         flags[1] |= (1 << (bit - 32));
    //     } else {
    //         flags[0] |= (1 << bit);
    //     }
    // }
    // void clearBit(int bit) {
    //     if (bit > 32) {
    //         flags[0] &= (1 << (bit - 32));
    //         flags[1] &= ~0;
    //     } else {
    //         flags[0] &= ~0;
    //         flags[1] &= (1 << bit);
    //     }
    // }
};

class dProcShareProc_c : public d3d::proc_c {
public:
    dProcShareProc_c() : idk(0) {}
    // virtual ~dProcShareProc_c() {}
    mAllocator_c mAlloc;
    u32 idk;
};

class dShareBgTexProc_c : public dProcShareProc_c {
public:
    dShareBgTexProc_c() {}
    // virtual ~dShareBgTexProc_c() {}
    virtual void remove();

    void create(bgTex_c *, EGG::Heap *);
};

struct dBgScrollLimit_c {
    float mL, mR, mU, mD;
    float mL2, mR2, mU2, mD2;
    float mL3, mR3, mU3, mD3;
    float mL4, mR4, mU4, mD4;
    u16 mFlags;
};

class dBgBound_c {
public:
    dBgBound_c() : mLeft(0.0f), mRight(0.0f), mUp(0.0f), mDown(0.0f) {}

    float mLeft, mRight, mUp, mDown;
};

struct dBgSomeInfo_c {
    dBgSomeInfo_c() :
        m_10(nullptr), m_14(nullptr), m_18(0), m_1c(0) {}

    dBgBound_c mBounds;
    dAcPy_c *m_10, *m_14;
    int m_18, m_1c, m_20, m_24, m_28, m_2c, m_30;
};

struct dBgSubstruct2_c {
public:
    float mFloat1, mFloat2, mFloat3, mFloat4;
    u8 mU8_1;
    u8 mU8_2;
    u8 mU8_3;
    u8 mU8_4;
};

struct dBgAnimObj_c {
    void set(dBgAnimObj_c obj) { *this = obj; }

    float mX, mY;
    u8 mType;
};

struct dHeapSize_c {
public:
    dHeapSize_c(size_t size) : mSize(size) {}

    size_t mSize;
};

struct sBgThing {
    u16 m_00;
    u16 m_02;
    int m_04;
    int m_08;
    int m_0c;
};;

class dBg_c : public dBase_c {
    class dBg_autoScroll_c {
    public:
        dBg_autoScroll_c() {}
        ~dBg_autoScroll_c() {}

        // char data[0x1c];
        mVec3_c mPos;
        float m_0c;
        float m_10;
        u8 m_14, m_15, m_16, m_17;
        u8 m_18;
        bool m_19;
        bool mActive;
    };

public:
    dBg_c();
    virtual ~dBg_c();

    void CreateBgCheckBuffer();

    unsigned long long fn_800774A0(u16);
    static unsigned long long fn_80081960(unsigned long long, int);
    dBgUnit_c *fn_80077520(u16 param_2, u16 param_3, u8 param_4, int *param_5, bool b);

    unsigned long long getBgCheckBuff(u16);
    unsigned long long *getpBgCheckBuff(u16);
    u16 GetUnitNumber(u16, u16, u8, bool);
    u16 GetMaskedUnitNumber(u16, u16, u8);
    unsigned long long GetBgCheckData(u16, u16, u8);
    u32 GetUnitTypeInfo(u16, u16, u8);
    u32 GetUnitKindInfo(u16, u16, u8);
    void CoinGetCommon(u16, u16, int, u16 *, u16 *, u16 *);
    void CoinGetBitSet(u16, u16, int);
    bool CoinGetBitCheck(u16, u16, int);
    void BgUnitChange(u16, u16, int, u16);
    bool CheckExistLayer(u8);
    void DispScaleCalc();
    void CreateBgTex();
    void RemoveBgTex();

    bgTex_c *__createBgTex(int, u16, u16, u16, u16, int, int);
    void fn_80078250(u16, u16, u16, u32);
    void fn_80078300();
    void fn_80077860(u16, u16, int, u16);
    void setWaterInWave(float, float, u8);
    void setBubble(float, float, u8, u8);

    void InitWmSwitchPushed();
    bool IsWmSwitchPushed();

    dPanelObjMgr_c *getPanelObjMgr(int idx);

    static u16 dBg_getUpLimitScroll(u8);
    static int dBg_getScrlAreaDataSize(u8);
    static sScrollAreaData *dBg_getScrlAreaDataP(u8, u8);
    bool dBg_isFlyPlayer();
    int dBg_isCloudFlyPlayer();
    bool dBg_isCloudFlyPlayerMulti(); // [Not static? bruh]

    float fn_80078860(float);

    float getMaxLeftPos();
    float getLeftLimit();
    float getMaxRightPos();
    float getRightLimit();
    float getLeftLimitSingle();
    float getLeftLimitMulti();
    float getZoomTargetMin_Common();
    float getZoomTargetMin();
    float getZoomTargetMid_Common();
    float getZoomTargetMid();
    float getZoomTargetMax_Common();
    float getZoomTargetMax();

    float getZoomSpreadLine();
    float getZoomSpreadLine2();
    float getZoomShrinkLine();
    float getZoomShrinkLine2();

    void calcLookatOffs();
    void calcLookatOffsLimit();
    void calcLookatOffsX(dActor_c *);
    void calcLookatOffsY(dActor_c *);

    void initAutoScroll();
    void calcLoopAutoScroll();
    void calcAutoScroll();
    void AutoScroll_start();
    void AutoScroll_stop();
    void calcScroll(const mVec3_c&, int);
    void calcMultiScroll( int );
    void initScroll();

    void fn_8007ac40(dBgSomeInfo_c *, int); ///< @unofficial
    void fn_8007ba70(dBgSomeInfo_c *); ///< @unofficial
    float fn_8007bba0(dBgSomeInfo_c *); ///< @unofficial
    float fn_8007bbf0(dBgSomeInfo_c *); ///< @unofficial
    bool fn_8007bc40(dBgSomeInfo_c *, float); ///< @unofficial
    mVec2_c fn_8007bd40(dBgSomeInfo_c *, float, float, float *); ///< @unofficial
    float fn_8007c7d0(float); ///< @unofficial
    void fn_8007ca90(dBgSomeInfo_c *, int, int); ///< @unofficial
    void fn_8007cd70(dBgSomeInfo_c *, dBgSomeInfo_c *, int); ///< @unofficial

    u8 freeUpScrollLimit(dBgScrollLimit_c *scrollLimit, int group, int area); ///< @unofficial
    u8 freeUpScrollLimit2(dBgScrollLimit_c *scrollLimit, int group, int area); ///< @unofficial

    void setScrollLimit(dBgScrollLimit_c *scrollLimit, int areaNo, int type, int group); ///< @unofficial

    void fn_8007E060(dBgSubstruct2_c *s, int idx);

    float getAreaUpLimitScroll();

    static void bg_createHeap();
    static void bg_destroyHeap();

    float getDispScale() { return mDispScale; }

    float getScaleFactor() { return 1.0f / mDispScale; }

    dBgSubstruct2_c * getData2(int idx, int i) { return &mData2[idx][i]; }

    typedef void (dBg_c::*callbackFunc)(dBgSomeInfo_c *, int);

    unsigned long long *mBgCheckBuffer;
    int mBgThingsRelated;
    sBgThing mBgThings[256];
    dBgScrollLimit_c mScrLimit[64][8][16];
    dBgSubstruct2_c mData2[64][20];
    int mGrassCount;
    int mFlowerCount;
    dBgAnimObj_c mGrass[100];
    dBgAnimObj_c mFlowers[100];
    u8 mUnused1[56];
    float m_8fda8;
    u8 mUnused2[40];
    float mBounds4[4];
    float mBounds3[4];
    float mL, mR, mU, mD;
    float mMoreFloats[1];
    mVec2_c mSomeSize;
    mVec2_c mPrevSomeSize;
    mVec2_c mSomePos;
    mVec2_c mPrevSomePos;
    float mMoreFloats6[2];
    float mLoopOffsetX;
    float mLoopOffsetY;
    mVec2_c mSomeParameterPos;
    float mMoreFloats7[7];
    float mWaveRelated[80];
    float mDispScale;
    float mMoreFloats3[4];
    u8 mU8s[5];
    float mZoomDenom;
    u8 m_8ffbc;
    float mLLimit, mRLimit, mULimit, mDLimit;
    float mLLimitSingle, mRLimitSingle, mULimitSingle, mDLimitSingle;
    float mLLimitMulti, mRLimitMulti, mULimitMulti, mDLimitMulti;
    float mPlayerPosY;
    int mIdkI;
    float mIdkF[4];
    u8 m_90008;
    u8 m_90009;
    u8 m_9000a;
    callbackFunc mCallbackFunc;
    float m_90018;
    int m_9001c;
    float m_90020;
    u8 m_90024;
    float mMoreFloats5[5];
    dBg_autoScroll_c mAutoscrolls[2];
    u8 mPad2[8];
    int m_9007c, m_90080;
    float m_90084;
    short m_90088, m_9008a;
    u8 m_9008c, m_9008d;
    u8 mLimitRelated;
    u8 mLimitRelated2;
    u8 m_90090, mScrollID, m_90092, m_90093;
    u8 mPad[12];
    float mZoomWeight;
    u8 mZoomRelated;
    u8 m_900a5, m_900a6, m_900a7, m_900a8;
    float m_900ac, m_900b0;
    u8 m_900b4;
    short m_900b6;
    float m_900b8, m_900bc;
    float mWaveX[20];
    float mWaveY[20];
    u8 mWaveType[20];
    bool mWaveActive[20];
    float mBubbleX[200];
    float mBubbleY[200];
    u8 mBubbleType[200];
    u8 mBubbleType2[200];
    short m_90958;
    u8 m_9095a, m_9095b, m_9095c, m_9095d;
    float m_90960;
    float m_90964;
    dBgSomeInfo_c mSomeInfo1;
    dBgSomeInfo_c mSomeInfo2;
    bgTex_c *mTex[3];
    dShareBgTexProc_c *mTexProc;
    dBgTexMng_c mTexMng;

    static dBg_c *m_bg_p;
    static dHeapSize_c m_HeapSize;
    static EGG::FrmHeap *m_FrmHeap_p;
};
