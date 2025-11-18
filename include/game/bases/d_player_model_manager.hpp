#pragma once

#include <game/bases/d_player_model_base.hpp>

class dPyAnm_HIO_c {
public:
    u8 mID;
    float mRate;
    float mBlendDuration;
};

class dPyAnmMain_HIO_c {
public:
    dPyAnm_HIO_c mAnm[177];
};

/// @unofficial
struct dPyModelData_s {
    float a, b, c, d;
};

class dPyModel_HIO_c {
public:
    float mData[5];

    dPyModelData_s getModelData(u8 index) {
        return modelData[index];
    }

    dPyModelData_s modelData[3];
};

class dYoshiModel_HIO_c {
public:
    float mData[4];
};

class dPyMdlBase_HIO_c {
public:
    u8 changeHioType(u8 hioType);
    float getValue(dPyModelData_s model, u8 powerup); ///< @unofficial

    float getValue(int playerType, int idx, int powerup) {
        dPyModelData_s modelData = mPyModel[changeHioType(playerType)].getModelData(idx);
        return getValue(modelData, powerup);
    }

    enum ScaleIndex_e {};

    float get_08(ScaleIndex_e index) {
        return m_08[index];
    }

    u8 mPad[0x8];
    float m_08[8];
    dPyAnmMain_HIO_c mPyAnm;
    dPyModel_HIO_c mPyModel[3];
    dYoshiModel_HIO_c mYoshiModel[3];
};

class dPyMdlMng_c {
public:
    enum ModelType_e {};
    enum SceneType_e {
        SCENE_TYPE_0
    };

    dPyMdlMng_c(ModelType_e modelType);
    virtual ~dPyMdlMng_c();
    void calc(mMtx_c &);
    void calc(mVec3_c, mAng3_c, mVec3_c);
    void calc2();
    void draw();
    void play();

    void create(u8 playerNo, u8 powerup, SceneType_e sceneType);

    static dPyAnm_HIO_c &getHIO(u8 anmID) {
        return m_hio.mPyAnm.mAnm[anmID];
    }

    void setAnm(int anmID, float rate, float blendDuration, float f) {
        mpMdl->setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, float blendDuration, float f) {
        float rate = getHIO(anmID).mRate;
        setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, const dPyAnm_HIO_c &hio, float f = 0.0f) {
        mpMdl->setAnm(anmID, hio.mRate, hio.mBlendDuration, f);
    }

    void setAnm(int anmID, float f = 0.0f) {
        setAnm(anmID, m_hio.mPyAnm.mAnm[(u8) anmID], f);
    }

    void setAnmOnlyRate(int anmID, const dPyAnm_HIO_c &hio, float a, float b) {
        mpMdl->setAnm(anmID, hio.mRate, a, b);
    }

    void setAnmOnlyRate(int anmID, float a, float b) {
        setAnmOnlyRate(anmID, getHIO(anmID), a, b);
    }

    bool isAnm(int anmID) const {
        return mpMdl->m_154 == anmID;
    }

    int getAnm() const {
        return mpMdl->m_154;
    }

    float getLastFrame() const {
        return mpMdl->mAnm.mFrameMax - 1.0f;
    }

    bool isAnmStop() const {
        return mpMdl->mAnm.isStop();
    }

    mAng3_c getAng() const {
        return mAng3_c(
            mpMdl->m_1fc,
            mpMdl->m_1fe,
            mpMdl->m_200
        );
    }
    void setAng(mAng3_c v) { mpMdl->setAng(v); }

    u32 getFlags() const {
        return mpMdl->mFlags;
    }

    u32 getFlags2() const {
        return mpMdl->m_17c;
    }

    mVec3_c &getHatPos() const { return mpMdl->mHatPosMaybe; }

    float getSomeScale() {
        return m_hio.get_08((dPyMdlBase_HIO_c::ScaleIndex_e) mpMdl->m_152);
    }

    float getSomeScale2() {
        return m_hio.m_08[mpMdl->m_152];
    }

    dPyMdlBase_c *mpMdl;
    u8 mPad[0x4];

    static dPyMdlBase_HIO_c *getHIO() {
        return &m_hio;
    }

    static dPyMdlBase_HIO_c m_hio;
};
