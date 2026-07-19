#pragma once

#include <game/bases/d_player_model_base.hpp>

class dYoshiMdl_c : public dPyMdlBase_c {
public:
    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallback_c(dYoshiMdl_c *owner) : mpOwner(owner) {}
        ~nodeCallback_c() override {}
        void timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl) override;
        void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) override;

        dYoshiMdl_c *mpOwner;
    };

    enum YoshiColor_e {};

    class mdl_inf_c {
    public:
        m3d::mdl_c mMdl;
    };

    dYoshiMdl_c(u8 modelType);
    ~dYoshiMdl_c() override;
    int getFaceJointIdx() const override { return 15; }
    void createModel() override;
    void initialize() override;
    void play() override;
    void _calc() override;
    void calc2() override;
    void draw() override;
    m3d::mdl_c *getBodyMdl() override { return &mMdl.mMdl; }
    const nw4r::g3d::ResFile *getAnmResFile() const override;
    void setFunbariRate(float) override;
    void getJointMtx(mMtx_c *mtx, int i) override;
    void setAnm(int animID, float rate, float c, float frame) override;
    bool setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int) override;
    void releaseBodyAnm(float) override;
    void _setFootAnm(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode, float rate, float frame, float blendFrame) override;
    void _setBodyAnm(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode, float rate, float frame, float blendFrame) override;
    void setAnmBind() override;
    void setFrame(float frame) override;
    void setBodyFrame(float frame) override;
    void setRate(float rate) override;
    void setBodyRate(float rate) override;

    void getPlayerObjectRes();
    void createPlayerModel();
    void getFootAnmTrans(mVec3_c *vec, const char *name);
    mVec3_c getTonguePos();
    mMtx_c getTongueTipMtx();
    void setTongueJointMtx(int jointIdx, const mMtx_c *mtx);
    mMtx_c *getTongueJointMtx(int jointIdx);
    void setTongueJointMtxBuf(int jointIdx, const mMtx_c *mtx);
    mMtx_c *getTongueJointMtxBuf(int jointIdx);

    void setTongueBendCalc();
    void setTongueBgCheck();

    YoshiColor_e getColor() { return (YoshiColor_e) mColor; }

    nw4r::g3d::ResFile m_20c;
    nw4r::g3d::ResFile m_210;
    nw4r::g3d::ResFile m_214;
    nw4r::g3d::ResAnmChr m_218;
    nw4r::g3d::ResAnmChr m_21c;
    m3d::anmTexPat_c mAnmTexPat;
    int mColor;
    nw4r::g3d::ResFile m_250;
    mdl_inf_c mMdl;
    int m_294;
    float m_298;
    float m_29c;
    int m_2a0;
    mMtx_c mTongueJointMtxs[7];
    mVec2_c m_3f4;
    nodeCallback_c mNodeCallback;

    static const float scTongueLengthMax;
};
