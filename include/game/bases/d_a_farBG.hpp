#pragma once

#include <game/bases/d_actor.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_effect.hpp>
#include <lib/egg/util/eggEffect.hpp>

/// @unofficial
bool fn_80081BE0(int, int, int);

/// @unofficial
int fn_80089030();

namespace d3d {
    void SetResTevColorAll(nw4r::g3d::ResMdl *, GXTevRegID, GXColor);
    void setNodeVisibility(m3d::bmdl_c *, int, int);
}

class mAABB_c : public nw4r::math::AABB {
};

class mFrustum_c : public nw4r::math::FRUSTUM {
public:
    bool intersectAABB(mAABB_c *);
    bool set(float, float, float, float, float, float, const mMtx_c&, bool);
};

inline bool isZero(float val) {
    return (std::fabs(val) <= FLT_EPSILON);
}
/////////


const float l_speed_ratiodt[] = {
    0.0f, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f, 1.0f, 0.0f, 1.2f,
    1.5f, 2.0f, 4.0f, 0.0f, 0.0f
};

class daFarBG_HIO_c {
public:
    float m_00;
    float m_04;
    float m_08;
    float m_0c;
    float m_10;
    float m_14;
    u8 m_18;
    u8 m_19;
    u8 m_1a;
    u8 m_1b;
    u8 m_1c;
    u8 m_1d;
    u8 m_1e;
    bool mShouldHideModel;
    u8 m_20;
    u8 m_21;
    bool mShouldNotSetCullingInfo;
    u8 m_23;
    u16 m_24;
    u8 m_26;
    u8 m_27;
    u8 m_28;
    u8 m_29;
    u8 m_pad_2a[2];
    u32 m_2c;
    u8 m_30;
    u8 m_pad_31[3];
    GXColor m_34;
    GXColor m_38;
    u8 m_3c;
    u8 m_3d;
    u8 m_3e;
    u8 m_3f[2][4][8];

    daFarBG_HIO_c();
    ~daFarBG_HIO_c() {}
};


class daFarBG_c : public dActor_c {
public:

    enum RepeatType_e {
        REPEAT_0,
        REPEAT_1,
        REPEAT_2,
        REPEAT_3,
        REPEAT_4,
        REPEAT_5,
        REPEAT_6,
        REPEAT_7,
    };

    enum ScrollBaseType_e {
        SCROLL_0,
        SCROLL_1,
        SCROLL_2,
    };

    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallback_c(daFarBG_c * parent) : mpParent(parent) {}

        void timingB(unsigned long nodeIdx, nw4r::g3d::WorldMtxManip* b, nw4r::g3d::ResMdl resMdl) override;

        daFarBG_c *mpParent;
    };

    class bgData_t {
        public:
        mVec3_c m_0;
        u32 m_c;

        bgData_t() {}
        ~bgData_t() {}
    };

    /// @unofficial
    struct sBgData {
        u16 mID;
        u16 mXScrollRate;
        u16 mYScrollRate;
        s16 mYOffset;
        s16 mXOffset;
        u16 mFile1;
        u16 mFile2;
        u16 mFile3;
        u8 mPad0[2];
        u16 mScale;
        u8 mPad1[4];
    };


    class mdlData_t {
    public:
        m3d::mdl_c * mModel;
        m3d::anmChr_c * mAnmChr;
        m3d::anmMatClr_c * mAnmClr;
        m3d::anmTexSrt_c * mAnmSrt;
        bool mIsTranslation;
        bool m_11;
        bool m_12[2];
        bool mIsEnabled;
        u8 mPad2[3];
        dEf::followEffect_c mFollowEffects[2];

        bool createKazanEffect(mVec3_c, int);
        bool updateKazanEffect(mVec3_c, int);
    };

    nodeCallback_c mCallback;
    dHeapAllocator_c mAllocator;
    bgData_t * mpBgData; ///< 3D array of 34 * 22 * 3 bgData_t entries
    sBgData * mpBgEntries[2];
    mdlData_t * mpBackgrounds[3][9];
    mdlData_t * mpStaticBackground;
    int mStaticBGIdx;
    mVec3_c m_438[6];
    mVec2_c mOffset;
    u8 mPad1[4];
    mVec2_c m_48c;
    u8 mPad2[4];
    mVec3_c m_498[6];
    RepeatType_e mRepeatType;
    int mIsBgB;
    float mBgScroll;
    float m_4ec;
    float m_4f0;
    mFrustum_c mFrustum;
    int m_5e4;
    u8 m_5e8[2];
    u8 m_5ea;
    u8 m_5eb;
    float mCenterXPos;
    u32 mCenterMode;
    u16 m_5f4[2];
    u16 m_5f8;
    u16 m_5fa;
    mMtx_c m_5fc;
    u8 m_62c;
    u8 m_62d;
    u8 mPad4[2];
    float m_630;
    float m_634;
    float m_638;
    u16 m_658[100];

    daFarBG_c() : mCallback(this) {}
    virtual ~daFarBG_c() {}

    int create() override;
    int execute() override;
    int draw() override;
    int doDelete() override;

    bool initialize();
    void CalcScrollBG();
    void UpdateAnim();
    void CalcBasePosAndMtx();
    void ReserveModel();
    void effectExecute();
    void InitFrustum();
    bool entryModel(mdlData_t *);
    void SetCullingInfo(m3d::smdl_c &);
    void SetBgDataP();
    sBgData * GetMyBgDataP();
    sBgData * GetOtherBgDataP();
    RepeatType_e GetRepeatType(u16, u16, u16);
    float GetHScrollSpdRatio(int);
    float GetVScrollSpdRatio(int);
    float GetScrollBaseY();
    ScrollBaseType_e GetScrollBaseType();
    float GetModelBasePosX(int, int, float);
    void fn_80116E60(int, int); ///< @unofficial
    void fn_80117030(int, int); ///< @unofficial
    void fn_801171F0(int, int); ///< @unofficial
    void fn_801173A0(int, int); ///< @unofficial
    void fn_80117550(int, int); ///< @unofficial
    void fn_80117710(int, int); ///< @unofficial
    void fn_801178F0(int, int); ///< @unofficial
    void InitBgData();
    void SetScrollFlag(int, nw4r::g3d::ResMdl);
    void CreateHeap();
    void CalcModelBasePos();
    bool checkResource();
    nw4r::g3d::ResFile GetRes(unsigned short bg_id, char* arcName, char* resPath, char* out_arcName);
    void CreateModel(unsigned short, daFarBG_c::mdlData_t*, bool);
    bool GetModelNumXY(int &, int &);
    float getXScrollRate(int);
    float getYScrollRate(int);
    void CalcScale();
    float GetZoomMagnif();
    mVec2_c GetScreenCenterPos();
    mVec3_c GetScrollDiff(int);
    bool fn_80119370(m3d::bmdl_c *mdl, ulong idx, nw4r::math::VEC3 &vec); ///< @unofficial
    mVec3_c GetCameraPos();

    static float c_PIC_WIDTH;
    static float c_PIC_WIDTH_HALF;
    static float c_PIC_HEIGHT;
    static float c_PIC_HEIGHT_HALF;

    static daFarBG_HIO_c m_HIO[2];
};

/// Probably belongs in a different TU
/// @unofficial
daFarBG_c::sBgData * fn_80081C40(int);


