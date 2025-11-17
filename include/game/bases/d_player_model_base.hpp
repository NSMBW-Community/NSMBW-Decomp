#pragma once

#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_mtx.hpp>

class dPyMdlBase_c {
public:
    virtual ~dPyMdlBase_c();
    virtual void getFaceJointIdx();
    virtual void createModel();
    virtual void initialize();
    virtual void play();
    virtual void _calc();
    virtual void calc2();
    virtual void draw();
    virtual void getBodyMdl();
    virtual void getAnmResFile();
    virtual void setPlayerMode();
    virtual void setColorType();
    virtual void setDark();
    virtual void vf3c(); ///< @unofficial
    virtual void onStarAnm();
    virtual void offStarAnm();
    virtual void onStarEffect();
    virtual void offStarEffect();
    virtual void getJointMtx();
    virtual void getHeadPropelJointMtx();
    virtual void vf58(); ///< @unofficial
    virtual void setAnm(int, float, float, float);
    virtual void vf60(); ///< @unofficial
    virtual void vf64(); ///< @unofficial
    virtual void vf68(); ///< @unofficial
    virtual void copyAnm();
    virtual void vf70(); ///< @unofficial
    virtual void vf74(); ///< @unofficial
    virtual void setAnmBind();
    virtual void vf7c(); ///< @unofficial
    virtual void setTexAnmType();
    virtual void setFrame(float);
    virtual void setBodyFrame();
    virtual void setRate(float);
    virtual void setBodyRate();
    virtual void vf94(); ///< @unofficial
    virtual void getPropelRollSpeed();
    virtual void vf9c(); ///< @unofficial
    virtual void vfa0(); ///< @unofficial
    virtual void vfa4(); ///< @unofficial
    virtual void vfa8(); ///< @unofficial
    virtual void vfac(); ///< @unofficial

    bool isFootStepTiming();
    void getJointPos(mVec3_c *, int);

    float getFrameMax() { return mAnm.mFrameMax; }
    mMtx_c &getMtx() { return mMtx; }

    void setAng(mAng3_c ang) {
        m_1fc = ang.x;
        m_1fe = ang.y;
        m_200 = ang.z;
    }

    u8 mPad1[0x24];
    m3d::anmChr_c mAnm;
    u8 mPad2[0x6c];
    mVec3_c mHatPosMaybe;
    u8 mPad3[0x30];
    mMtx_c mMtx;
    u8 mPad4[0x1a];
    u8 m_152;
    int m_154;
    u8 mPad5[8];
    u32 mFlags;
    u8 mPad6[0x98];
    s16 m_1fc;
    s16 m_1fe;
    s16 m_200;
    u32 m_204;
    u32 m_208;

    static const float scWaterCrouchAnmSpeed;
};
