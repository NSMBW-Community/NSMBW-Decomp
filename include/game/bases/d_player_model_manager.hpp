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

class dPyModel_HIO_c {
public:
    float mData[17];
};

class dYoshiModel_HIO_c {
public:
    float mData[4];
};

class dPyMdlBase_HIO_c {
public:
    u8 mPad[0x8];
    float m_08[8];
    dPyAnmMain_HIO_c mPyAnm;
    dPyModel_HIO_c mPyModel[3];
    dYoshiModel_HIO_c mYoshiModel;
};

class dPyMdlMng_c {
public:
    enum ModelType_e {};

    dPyMdlMng_c(ModelType_e modelType);
    virtual ~dPyMdlMng_c();

    void setAnm(int anmID, float rate, float blendDuration, float f) {
        mpMdl->setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, float blendDuration, float f) {
        float rate = m_hio.mPyAnm.mAnm[anmID].mRate;
        setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, const dPyAnm_HIO_c &hio, float f = 0.0f) {
        mpMdl->setAnm(anmID, hio.mRate, hio.mBlendDuration, f);
    }

    void setAnm(int anmID, float f = 0.0f) {
        setAnm(anmID, m_hio.mPyAnm.mAnm[anmID], f);
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

    dPyMdlBase_c *mpMdl;
    u8 mPad[0x4];

    static dPyMdlBase_HIO_c m_hio;
};
