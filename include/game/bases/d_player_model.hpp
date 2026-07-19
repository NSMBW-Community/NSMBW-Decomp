#pragma once

#include <game/bases/d_player_model_base.hpp>
#include <game/mLib/m_effect.hpp>

#include <constants/game_constants.h>

class dPlayerMdl_c : public dPyMdlBase_c {
public:
    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallback_c(dPlayerMdl_c *owner) : mpOwner(owner) {}
        ~nodeCallback_c() override {}
        void timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl) override;
        void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) override;

        dPlayerMdl_c *mpOwner;
    };
    class nodeCallbackPLH_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallbackPLH_c(dPlayerMdl_c *owner) : mpOwner(owner) {}
        ~nodeCallbackPLH_c() override {}
        void timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl) override;
        void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) override;

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

    ///< @unofficial
    struct ModelInfo_s {
        const char * mModelArcName;
        const char * mAnimArcName;
        const char * mModelNameBody;
        const char * mModelNameBodySmall;
        const char * mModelNameBodyPropeller;
        const char * mModelNameBodyPenguin;
        const char * mModelNameHead;
        const char * mModelNameHeadSmall;
        const char * mModelNameHeadPropeller;
        const char * mModelNameHeadPenguin;
        float m_28;
        float m_2c;
        float m_30;
        float m_34;
    };

    dPlayerMdl_c(u8);
    ~dPlayerMdl_c() override;
    int getFaceJointIdx() const override { return m_77c; }
    void createModel() override;
    void initialize() override;
    void play() override;
    void _calc() override;
    void calc2() override;
    void draw() override;
    m3d::mdl_c *getBodyMdl() override { return &mInfo[mPyPlayerMode].mMdl1; }
    const nw4r::g3d::ResFile *getAnmResFile() const override { return &m_210; }
    void setPlayerMode(int mode) override;
    void setColorType(u8 type) override;
    void setDark(int) override;
    void onStarAnm() override;
    void offStarAnm() override;
    bool getHeadPropelJointMtx(mMtx_c *mtx) override;
    bool getJumpAnmName(int jumpType, char *anmNameBuf, int p4) override;
    void copyAnm() override;
    void _setFootAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float) override;
    void _setBodyAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float) override;
    void setAnmBind() override;
    bool setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int) override;
    bool setPersonalRideAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr) override;
    void setTexAnmType(TexAnmType_e anmType) override;
    void setPropelRollSpeed(s16 speed) override; // [TODO: implement as inlines]
    s16 getPropelRollSpeed() const override; // [TODO: implement as inlines]
    void setPropelRollAngle(s16 speed) override; // [TODO: implement as inlines]
    s16 getPropelRollAngle() const override; // [TODO: implement as inlines]
    void setPropelScale(float scale) override; // [TODO: implement as inlines]
    float *getLegLengthP(u8) override; // [TODO: implement as inlines]

    virtual void FUN_800d4750(int mode); ///< @unofficial
    virtual void callbackTimingA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResNode resNode);
    virtual float getAimMotionShareScale() { return 1.0f; }
    virtual void createPlayerModel();

    void setInitTexAnm();
    void _setHeadTexAnm(nw4r::g3d::ResAnmTexPat &anmTexPat, m3d::playMode_e playMode, float rate, float frame);
    void resetTexAnmType();
    m3d::anmTexPat_c &getHeadTexAnm();
    void getPlayerObjectRes();
    void changeLightTexture(m3d::mdl_c *mdl, _GXTexMapID texMapID);
    void setStarAnm(m3d::mdl_c *mdl, m3d::anmMatClr_c *matClr);
    void removeStarAnm(m3d::mdl_c *mdl);
    void calcBlendRate();
    void setStarEffect();

    m3d::mdl_c &getMdl1() { return mInfo[mPyPlayerMode].mMdl1; }
    m3d::mdl_c &getMdl2() { return mInfo[mPyPlayerMode].mMdl2; }

    nw4r::g3d::ResFile m_20c;
    nw4r::g3d::ResFile m_210;
    nw4r::g3d::ResFile m_214;
    mEf::levelEffect_c mLevelEf1;
    mEf::levelEffect_c mLevelEf2;
    mdl_inf_c mInfo[PLAYER_MODE_COUNT];
    m3d::anmTexPat_c mTexAnm1;
    m3d::anmMatClr_c mMatClrAnm1;
    m3d::anmTexPat_c mTexAnm2;
    m3d::anmTexPat_c mTexAnmPropeller;
    m3d::anmTexPat_c mTexAnmPenguin;
    m3d::anmMatClr_c mMatClrAnm2;
    PlayerMode_e mPyPlayerMode;
    PlayerMode_e mOtherPlayerMode;
    const ModelInfo_s *mpArcNames;
    u32 m_77c;
    s16 m_780;
    s16 m_782;
    float m_784;
    mVec3_c mMotionShareScale;
    nodeCallback_c mCallback;
    nodeCallbackPLH_c mCallbackPLH;
    other_inf_s mOtherInf2[4];
    other_inf_s mOtherInf1[4];

    static const int scPropelRollBaseSpeed;
    static const TexAnmData_s scTexAnmData[TEX_ANM_COUNT];
};
