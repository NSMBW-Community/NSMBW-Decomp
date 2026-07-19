#pragma once

#include <game/bases/d_player_model_base.hpp>
#include <game/bases/d_mario_mdl.hpp>
#include <game/bases/d_kinopio_mdl.hpp>
#include <game/bases/d_yoshi_mdl.hpp>

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
    float mData1;
    float mData2[4];

    float getValue(u8 playerMode) {
        return mData2[playerMode];
    }

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
    dPyMdlBase_HIO_c();
    ~dPyMdlBase_HIO_c();

    u8 changeHioType(u8 hioType);
    float getValue(dPyModelData_s model, u8 powerup); ///< @unofficial

    float getOffsetTarget(int playerType, u8 type) {
        u8 t = changeHioType(playerType);
        return mPyModel[t].mData2[type];
    }

    float getValue(int playerType, int idx, int powerup) {
        dPyModelData_s modelData = mPyModel[changeHioType(playerType)].getModelData(idx);
        return getValue(modelData, powerup);
    }

    enum ScaleIndex_e {};

    float get_04() {
        return m_04;
    }

    float get_08(ScaleIndex_e index) {
        return m_08[index];
    }

    u8 mPad[0x4];
    float m_04;
    float m_08[7];
    u8 mPad2[0x4];
    dPyAnmMain_HIO_c mPyAnm;
    dPyModel_HIO_c mPyModel[3];
    dYoshiModel_HIO_c mYoshiModel;
};

class dPyMdlMng_c {
public:
    enum ModelType_e {
        TYPE_MARIO1,
        TYPE_MARIO2,
        TYPE_TOAD1,
        TYPE_TOAD2,
        TYPE_TOAD3,
        TYPE_YOSHI,
    };
    enum SceneType_e {
        SCENE_TYPE_0
    };

    dPyMdlMng_c(ModelType_e modelType);

    virtual ~dPyMdlMng_c();
    void construct(u8 modelType);
    void create(u8 playerNo, u8 powerup, SceneType_e sceneType);

    void play();
    void calc(mMtx_c &mtx);
    void calc(mVec3_c pos, mAng3_c rot, mVec3_c scale);
    void calc2();
    void draw();

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
        return mpMdl->mCurrAnmID == anmID;
    }

    int getAnm() const {
        return mpMdl->mCurrAnmID;
    }

    float getLastFrame() const {
        return mpMdl->getFootAnm().mFrameMax - 1.0f;
    }

    bool isAnmStop() const {
        return mpMdl->getFootAnm().isStop();
    }

    mAng3_c getAng() const {
        return mAng3_c(
            mpMdl->mAngX,
            mpMdl->mAngY,
            mpMdl->mAngZ
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
        return m_hio.get_08((dPyMdlBase_HIO_c::ScaleIndex_e) mpMdl->mPlayerMode);
    }

    float getSomeScale2() {
        return m_hio.m_08[mpMdl->mPlayerMode];
    }

    dPyMdlBase_c *mpMdl;
    u32 m_0c;

    static dPyMdlBase_HIO_c *getHIO() {
        return &m_hio;
    }

    static dPyMdlBase_HIO_c m_hio;
};
