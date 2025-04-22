#include <game/mLib/m_3d.hpp>

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

// Computes 1 / n efficiently with fres + 1 Newton iteration
inline float FInv(register float n) {
    register float x0, tmp1, tmp2;
    asm {
        // Approximate with fres, then use Newton iteration (https://cr.yp.to/2005-590/powerpc-cwg.pdf 3.3.6.3)
        // https://en.wikipedia.org/wiki/Division_algorithm#Newton%E2%80%93Raphson_division
        // Want to calculate x_1 = 2 * x_0 - n * x_0 ^ 2

        fres x0, n

        ps_add tmp1, x0, x0 // tmp1 = 2 * x_0
        ps_mul tmp2, x0, x0 // tmp2 = x_0 ^ 2
        ps_nmsub x0, n, tmp2, tmp1 // x_1 = -(n * tmp2 - tmp1) = tmp1 - n * tmp2
    }

    return x0;
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
        float inv1 = FInv(divRes + subRes);
        float inv2 = FInv(divRes + subRes);

        mScaleFrom = divRes * inv1;
        mScaleTo = subRes * inv1;
        mInterpolateT = subRes * inv2;
    }
}

bool m3d::calcRatio_c::isEnd() const {
    return mWeight == 0;
}
