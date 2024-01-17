#include <dol/mLib/m_3d.hpp>

m3d::calcRatio_c::calcRatio_c() {
    f1 = 0;
    f2 = 1;
    f3 = 0;
    mScaleSel = 0;
    mScaleAnm = 1;
    mSlerpParam = 1;
    b1 = false;
    b2 = false;
}

void m3d::calcRatio_c::remove() {
    b1 = false;
    reset();
}

void m3d::calcRatio_c::reset() {
    f1 = 0;
    f2 = 1;
    mScaleSel = 0;
    mScaleAnm = 1;
    mSlerpParam = 1;
}

void m3d::calcRatio_c::offUpdate() {
    b1 = true;
    b2 = false;
}

void m3d::calcRatio_c::set(float v) {
    if (v == 0) {
        reset();
    } else {
        f1 = 1;
        f2 = 0;
        mScaleSel = 1;
        mScaleAnm = 0;
        mSlerpParam = 0;
        b2 = true;
        f3 = 1.0f / v;
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
    if (f1 == 0.0f) {
        return;
    }
    f2 += f3;
    if (f2 >= 1.0f) {
        reset();
    } else {
        b2 = true;

        float fVar2 = f1 - f1 * f2 * f2;
        float prevF1 = f1;
        f1 = fVar2;

        float divRes = fVar2 / prevF1;
        float subRes = 1.0f - fVar2;

        float inv1 = FInv(divRes + subRes);
        float inv2 = FInv(divRes + subRes);
        
        mScaleSel = divRes * inv1;
        mScaleAnm = subRes * inv1;
        mSlerpParam = subRes * inv2;
    }
}

bool m3d::calcRatio_c::isEnd() const {
    return f1 == 0;
}
