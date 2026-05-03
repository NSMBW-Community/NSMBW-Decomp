#pragma once

#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_mtx.hpp>

class dAcPy_c;

class dPyMdlBase_c {
public:
    virtual ~dPyMdlBase_c();
    virtual int getFaceJointIdx();
    virtual void createModel();
    virtual void initialize();
    virtual void play();
    virtual void _calc();
    virtual void calc2();
    virtual void draw();
    virtual void getBodyMdl();
    virtual void getAnmResFile();
    virtual void setPlayerMode(int);
    virtual void setColorType(u8);
    virtual void setDark(int);
    virtual void setFunbariRate(float);
    virtual void onStarAnm();
    virtual void offStarAnm();
    virtual void onStarEffect();
    virtual void offStarEffect();
    virtual void getJointMtx(mMtx_c *, int);
    virtual void getHeadPropelJointMtx(mMtx_c *);
    virtual void vf58(); ///< @unofficial
    virtual void setAnm(int, float, float, float);
    virtual void setPersonalAnm(int, nw4r::g3d::ResAnmChr *, int);
    virtual void setBodyAnm(int, float, float, float);
    virtual void releaseBodyAnm(float);
    virtual void copyAnm();
    virtual void _setFootAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float);
    virtual void _setBodyAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float);
    virtual void setAnmBind();
    virtual void setPersonalRideAnm(int, nw4r::g3d::ResAnmChr *);
    virtual void setTexAnmType();
    virtual void setFrame(float);
    virtual void setBodyFrame();
    virtual void setRate(float);
    virtual void setBodyRate();
    virtual void setPropelRollSpeed(s16) const;
    virtual s16 getPropelRollSpeed();
    virtual void setPropelRollAngle(s16);
    virtual s16 getPropelRollAngle() const;
    virtual void setPropelScale(float);
    virtual float *getLegLengthP(u8);
    virtual void updateBonusCap();

    bool isFootStepTiming();
    void getJointPos(mVec3_c *, int);
    void fn_800d5e00(int);
    void copyLinkAnm(float f);

    float getFrameMax() { return mAnm.mFrameMax; }
    mMtx_c &getMtx() { return mMtx; }

    void setAng(mAng3_c ang) {
        m_1fc = ang.x;
        m_1fe = ang.y;
        m_200 = ang.z;
    }

    u32 get151CheckVal() { return 2; }

    u8 mPad1[0x1c];
    dAcPy_c *mpOwner;
    dPyMdlBase_c *mpSpinLiftParentMdl;
    m3d::anmChr_c mAnm;
    u8 mPad3[0x6c];
    mVec3_c mHatPosMaybe;
    u8 mPad4[0x30];
    mMtx_c mMtx;
    u8 mPad5[0xc];
    mVec3_c mScale;
    u8 mPad6[0x1];
    u8 m_151;
    u8 m_152;
    int mCurrAnmID;
    u8 mPad7[8];
    u32 mFlags;
    u8 mPad8[0x18];
    u32 m_17c;
    u8 mPad9[0x7c];
    s16 m_1fc;
    s16 m_1fe;
    s16 m_200;
    u32 m_204;
    u32 m_208;

    static const float scWaterCrouchAnmSpeed;
    static const float scFireShootFrame;
};
