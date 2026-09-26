#pragma once
#include <game/bases/d_effect.hpp>
#include <game/bases/d_ice_effect_scale.hpp>

class dIceEfInf_c {
public:
    dIceEfInf_c() : mScale(1.0f, 1.0f, 1.0f) {}

    virtual bool create(const mVec3_c &pos) = 0;
    virtual bool follow(const mVec3_c &pos) = 0;

    mVec3_c mScale;
};

class dIceFreezeEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return mEffect.follow(&pos, nullptr, &mScale); }
    bool create(const mVec3_c &pos) override { return mEffect.createEffect("Wm_ob_iceattack", 0, &pos, nullptr, &mScale); }

    dEf::followEffect_c mEffect;
};

class dIceSmokeEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { mEffect.createEffect("Wm_ob_icewait", 0, &pos, nullptr, &mScale); return true; }
    bool create(const mVec3_c &pos) override { return follow(pos); }

    mEf::levelEffect_c mEffect;
};

class dIceBreakEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return mEffect.follow(&pos, nullptr, &mScale); }
    bool create(const mVec3_c &pos) override { return mEffect.createEffect("Wm_ob_icehit", 0, &pos, nullptr, &mScale); }

    dEf::followEffect_c mEffect;
};

class dIceReleaseEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return mEffect.follow(&pos, nullptr, &mScale); }
    bool create(const mVec3_c &pos) override { return mEffect.createEffect("Wm_ob_icebreak", 0, &pos, nullptr, &mScale); }

    dEf::followEffect_c mEffect;
};

class dIceThawEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return mEffect.follow(&pos, nullptr, &mScale); }
    bool create(const mVec3_c &pos) override { return mEffect.createEffect("Wm_ob_icethaw", 0, &pos, nullptr, &mScale); }

    dEf::followEffect_c mEffect;
};

class dIceYoganEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return mEffect.follow(&pos, nullptr, &mScale); }
    bool create(const mVec3_c &pos) override { return mEffect.createEffect("Wm_ob_iceevaporate", 0, &pos, nullptr, &mScale); }

    dEf::followEffect_c mEffect;
};

class dIcePoisonEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return mEffect.follow(&pos, nullptr, &mScale); }
    bool create(const mVec3_c &pos) override { return mEffect.createEffect("Wm_ob_icepoison", 0, &pos, nullptr, &mScale); }

    dEf::followEffect_c mEffect;
};

class dIceWaterBreakEf_c : public dIceEfInf_c {
public:
    bool follow(const mVec3_c &pos) override { return false; }
    bool create(const mVec3_c &pos) override { return mEf::createEffect("Wm_ob_waterbreak", 0, &pos, nullptr, &mScale); }
};

class daIce_c;

class dIceEfMaker_c {
public:
    enum EfKind_e {
        ICE_EF_FREEZE,
        ICE_EF_SMOKE,
        ICE_EF_BREAK,
        ICE_EF_RELEASE,
        ICE_EF_THAW,
        ICE_EF_YOGAN,
        ICE_EF_POISON,
        ICE_EF_WATER_BREAK,
        ICE_EF_COUNT
    };

    dIceEfMaker_c(daIce_c *owner) : m_00(0), mpOwner(owner) {
        mpEffects[ICE_EF_FREEZE] = &mFreezeEffect;
        mpEffects[ICE_EF_SMOKE] = &mSmokeEffect;
        mpEffects[ICE_EF_BREAK] = &mBreakEffect;
        mpEffects[ICE_EF_RELEASE] = &mReleaseEffect;
        mpEffects[ICE_EF_THAW] = &mThawEffect;
        mpEffects[ICE_EF_YOGAN] = &mYoganEffect;
        mpEffects[ICE_EF_POISON] = &mPoisonEffect;
        mpEffects[ICE_EF_WATER_BREAK] = &mWaterBreakEffect;
    }

    void init(int type, dIceEfScale_c *scale);
    void execute();
    void fin();
    void createEffect(EfKind_e kind);
    void hahenEffect();

    int m_00;
    u8 mPad4[0x4];
    dIceFreezeEf_c mFreezeEffect;
    dIceSmokeEf_c mSmokeEffect;
    dIceBreakEf_c mBreakEffect;
    dIceReleaseEf_c mReleaseEffect;
    dIceThawEf_c mThawEffect;
    dIceYoganEf_c mYoganEffect;
    dIcePoisonEf_c mPoisonEffect;
    dIceWaterBreakEf_c mWaterBreakEffect;

    dIceEfInf_c *mpEffects[ICE_EF_COUNT];
    daIce_c *mpOwner;
};
