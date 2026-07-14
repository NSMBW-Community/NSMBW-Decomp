#pragma once

#include <game/bases/d_player_model_base.hpp>
#include <game/mLib/m_effect.hpp>

#include <constants/game_constants.h>

class dPlayerMdl_c : public dPyMdlBase_c {
public:
    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallback_c(dPlayerMdl_c *owner) : mpOwner(owner) {}
        virtual void timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl);
        virtual void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl);

        dPlayerMdl_c *mpOwner;
    };
    class nodeCallbackPLH_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallbackPLH_c(dPlayerMdl_c *owner) : mpOwner(owner) {}
        virtual void timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl);
        virtual void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl);

        dPlayerMdl_c *mpOwner;
    };

    class mdl_inf_c {
    public:
        m3d::mdl_c mMdl1;
        m3d::mdl_c mMdl2;
    };

    ///< @unofficial
    struct other_inf_s {
        float len1;
        float len2;
        float lensq1;
        float lensq2;
    };

    dPlayerMdl_c(u8);
    ~dPlayerMdl_c() override;
    void createModel() override;
    void initialize() override;
    void play() override;
    void _calc() override;
    void calc2() override;

    virtual void FUN_800d4750(); ///< @unofficial
    virtual void callbackTimingA();
    virtual float getAimMotionShareScale() { return 1.0f; }
    virtual void createPlayerModel();

    void getPlayerObjectRes();
    void setInitTexAnm();
    void calcBlendRate();
    void setStarEffect();
    m3d::anmTexPat_c *getHeadTexAnm();

    nw4r::g3d::ResFile m_20c;
    nw4r::g3d::ResFile m_210;
    nw4r::g3d::ResFile m_214;
    mEf::levelEffect_c mLevelEf1;
    mEf::levelEffect_c mLevelEf2;
    mdl_inf_c mInfo[4];
    m3d::anmTexPat_c mTexAnm1;
    m3d::anmMatClr_c mMatClrAnm1;
    m3d::anmTexPat_c mTexAnm2;
    m3d::anmTexPat_c mTexAnm3;
    m3d::anmTexPat_c mTexAnm4;
    m3d::anmMatClr_c mMatClrAnm2;
    u32 m_770;
    u32 m_774;
    const char **mpArcNames; ///< [This is wrong, seems to be some struct]
    u32 m_77c;
    s16 m_780;
    s16 m_782;
    float m_784;
    mVec3_c mMotionShareScale;
    nodeCallback_c mCallback;
    nodeCallbackPLH_c mCallbackPLH;
    other_inf_s mOtherInf2[4];
    other_inf_s mOtherInf1[4];
};
