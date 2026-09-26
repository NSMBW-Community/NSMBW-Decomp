#pragma once

class dIceEfScale_c {
public:
    dIceEfScale_c() :
        mScaleFreeze(0.0f), mScaleSmoke(0.0f),
        mScaleBreak(0.0f), mScaleRelease(0.0f),
        mScaleThaw(0.0f), mScaleYogan(0.0f),
        mScalePoison(0.0f), mScaleWaterBreak(0.0f) {}

    dIceEfScale_c(const dIceEfScale_c &other) :
        mScaleFreeze(other.mScaleFreeze), mScaleSmoke(other.mScaleSmoke),
        mScaleBreak(other.mScaleBreak), mScaleRelease(other.mScaleRelease),
        mScaleThaw(other.mScaleThaw), mScaleYogan(other.mScaleYogan),
        mScalePoison(other.mScalePoison), mScaleWaterBreak(other.mScaleWaterBreak) {}

    bool operator==(const dIceEfScale_c &other) const {
        return mScaleFreeze == other.mScaleFreeze &&
               mScaleSmoke == other.mScaleSmoke &&
               mScaleBreak == other.mScaleBreak &&
               mScaleRelease == other.mScaleRelease &&
               mScaleThaw == other.mScaleThaw &&
               mScaleYogan == other.mScaleYogan &&
               mScalePoison == other.mScalePoison &&
               mScaleWaterBreak == other.mScaleWaterBreak;
    }

    bool operator!=(const dIceEfScale_c &other) const {
        return !(*this == other);
    }

    float mScaleFreeze;
    float mScaleSmoke;
    float mScaleBreak;
    float mScaleRelease;
    float mScaleThaw;
    float mScaleYogan;
    float mScalePoison;
    float mScaleWaterBreak;

    static dIceEfScale_c Zero;
};
