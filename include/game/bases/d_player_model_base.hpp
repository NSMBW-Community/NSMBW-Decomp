#pragma once

#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/bases/d_heap_allocator.hpp>

class dAcPy_c;

class dPyMdlBase_c {
public:

    // The randomness distribution according to which the jump animation is
    // chosen in dPyMdlBase_c::setJumpAnmRand
    enum RndType_e {
        RND_EQUAL,  //< 33% 0, 33% 1, 33% 2
        RND_WEIGHTED,  //< 60% 0, 30% 1, 10% 2
    };

    enum TexAnmType_e {
        TEX_ANM_0,
        TEX_ANM_1,
        TEX_ANM_2,
        TEX_ANM_3,
        TEX_ANM_4,
        TEX_ANM_5,
        TEX_ANM_6,
        TEX_ANM_7,
        TEX_ANM_8,
        TEX_ANM_9,
        TEX_ANM_A,
        TEX_ANM_B,
        TEX_ANM_C,
        TEX_ANM_D
    };

    struct AnmData_s {
        char * m_00;
        char * m_04;
        char * m_08;
        char * m_0c;
        m3d::playMode_e play_mode;
        float m_14;
        float m_18;
        TexAnmType_e m_1c;
        u32 m_20;
    };

    dPyMdlBase_c(u8);
    virtual ~dPyMdlBase_c() {};
    virtual int getFaceJointIdx() const;
    virtual void createModel();
    virtual void initialize();
    virtual void play();
    virtual void _calc();
    virtual void calc2();
    virtual void draw();
    virtual m3d::bmdl_c * getBodyMdl() { return nullptr; }
    virtual const nw4r::g3d::ResFile * getAnmResFile();
    virtual void setPlayerMode(int);
    virtual void setColorType(u8);
    virtual void setDark(int);
    virtual void setFunbariRate(float) {};
    virtual void onStarAnm();
    virtual void offStarAnm();
    virtual void onStarEffect() { m_178 |= 0x200; };
    virtual void offStarEffect() { m_178 &= ~0x200; };
    virtual void getJointMtx(mMtx_c* mtx, int i);
    virtual bool getHeadPropelJointMtx(mMtx_c *);
    virtual bool getJumpAnmName(int, char *, int); //< @unofficial
    virtual void setAnm(int, float, float, float);
    virtual int setPersonalAnm(int, nw4r::g3d::ResAnmChr *, int);
    virtual void setBodyAnm(int, float, float, float);
    virtual void releaseBodyAnm(float);
    virtual void copyAnm();
    virtual void _setFootAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float);
    virtual void _setBodyAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float);
    virtual void setAnmBind();
    virtual int setPersonalRideAnm(int, nw4r::g3d::ResAnmChr *);
    virtual void setTexAnmType(TexAnmType_e);
    virtual void setFrame(float);
    virtual void setBodyFrame(float);
    virtual void setRate(float);
    virtual void setBodyRate(float);
    virtual void setPropelRollSpeed(s16);
    virtual s16 getPropelRollSpeed() const;
    virtual void setPropelRollAngle(s16);
    virtual s16 getPropelRollAngle() const;
    virtual void setPropelScale(float);
    virtual float *getLegLengthP(u8);
    virtual void updateBonusCap() {};

    int create(u8, u8, int);
    void calc(mMtx_c &);
    void calc(mVec3_c pos, mAng3_c rot, mVec3_c scale);
    void setBaseMtx(mVec3_c & pos, mAng3_c & rot, mVec3_c & scale);
    void setSoftLight(m3d::bmdl_c&);
    bool isBodyAnmOn();
    void setJumpAnmRand(RndType_e rnd_type);
    void setSlopeBodyAnm(float);
    void setCarryBodyAnm(float);
    void setLinkAnm(int, float, float, float);
    void setRideAnm(int, float, float, float);

    void calcStoopOffset();

    bool isFootStepTiming();
    void getJointPos(mVec3_c *, int);
    void copyLinkAnm(float f);

    float getFrameMax() { return mAnms[0].mFrameMax; }
    mMtx_c &getMtx() { return mMtx; }

    void setAng(mAng3_c ang) {
        m_1fc = ang.x;
        m_1fe = ang.y;
        m_200 = ang.z;
    }

    u32 get151CheckVal() { return 2; }

    dHeapAllocator_c mAllocator;
    dAcPy_c *mpOwner;
    dPyMdlBase_c *mpSpinLiftParentMdl;
    m3d::anmChr_c mAnms[2];
    m3d::anmChrBlend_c mBlendAnm;
    mVec3_c mHeadPosMaybe;
    mVec3_c mHatPosMaybe;
    mMtx_c mBaseMtx;
    mMtx_c mMtx;
    mVec3_c mHeadOffset;
    mVec3_c mScale;
    u8 m_another_player_ID;
    u8 m_151;
    u8 mPlayerMode;
    u8 m_powerup_tex;
    int mCurrAnmID;
    int mPrevAnmID;
    int m_158;
    u32 mFlags;
    u32 m_160;
    u32 m_164;
    u32 m_168;
    u32 mCurrHeadPatID;
    u32 mNextPatSwitchTimer;
    u8 m_174;
    /* 3 padding bytes */
    u32 m_178;
    u32 m_17c;
    float m_180;
    float m_184;
    int m_188_countdown;
    mVec3_c m_18c_vecs[6];
    u8 mPad9[0x24];
    u16 m_1f8;
    u16 m_1fa;
    s16 m_1fc;
    s16 m_1fe;
    u32 m_200;
    u32 m_204;

    static const float scWaterCrouchAnmSpeed;
    static const float scFireShootFrame;
    static const AnmData_s scPyAnmData[177];
    static const u32 lbl_802f2f38[2][2];
    static const char * scJumpAnmVarDt[3];
    static const char * sc2JumpAnmVarDt[3];
    static const char * sc2JumpedAnmVarDt[3];
};
