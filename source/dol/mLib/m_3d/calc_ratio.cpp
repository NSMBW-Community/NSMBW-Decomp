#include <game/mLib/m_3d.hpp>
#include <nw4r/math.h>

m3d::calcRatio_c::calcRatio_c() {
    mWeight = 0;
    mT = 1;
    mTimeStep = 0;

    mScaleFrom = 0;
    mScaleTo = 1;
    mInterpolateT = 1;

    mIsActive = false;
    mIsBlending = false;
}

void m3d::calcRatio_c::remove() {
    mIsActive = false;
    reset();
}

void m3d::calcRatio_c::reset() {
    mWeight = 0;
    mT = 1;

    mScaleFrom = 0;
    mScaleTo = 1;
    mInterpolateT = 1;
}

void m3d::calcRatio_c::offUpdate() {
    mIsActive = true;
    mIsBlending = false;
}

void m3d::calcRatio_c::set(float duration) {
    if (duration == 0) {
        reset();
    } else {
        mWeight = 1;
        mT = 0;
        mScaleFrom = 1;
        mScaleTo = 0;
        mInterpolateT = 0;

        mIsBlending = true;
        mTimeStep = 1.0f / duration;
    }
}

void m3d::calcRatio_c::calc() {
    if (mWeight == 0.0f) {
        return;
    }
    mT += mTimeStep;
    if (mT >= 1.0f) {
        reset();
    } else {
        mIsBlending = true;

        float prevWeight = mWeight;
        mWeight -= mWeight * mT * mT;

        float divRes = mWeight / prevWeight;
        float subRes = 1.0f - mWeight;

        // [Doing this twice is required for matching]
        float inv1 = nw4r::math::FInv(divRes + subRes);
        float inv2 = nw4r::math::FInv(divRes + subRes);

        mScaleFrom = divRes * inv1;
        mScaleTo = subRes * inv1;
        mInterpolateT = subRes * inv2;
    }
}

bool m3d::calcRatio_c::isEnd() const {
    return mWeight == 0;
}
