#include <types.h>
#include <game/framework/f_helper_unk.hpp>

void fBaHelper_c::Delete() {
    if (mpArr == nullptr || mStatus == 0) {
        return;
    }

    mCount--;
    while (mCount >= 0) {
        vf_0x10(mpArr[mCount]);
        mCount--;
    }
}

int fBaHelper_c::Load(int value) {
    if (mpArr == nullptr || mStatus == 2) {
        return 2;
    }

    while (mpArr[mCount]) {
        mStatus = vf_0xc(mpArr[mCount]);
        if (mStatus != 2) break;
        mCount++;
        mUnknown = 0;
        if (value == 0) break;
    }

    return mStatus;
}

bool fBaHelper_c::LoadOnlyOne() {
    if (mStatus == 1) {
        mStatus = Load(0);
    }

    if (mStatus == 1) {
        return false;
    } else {
        return true;
    }
}
