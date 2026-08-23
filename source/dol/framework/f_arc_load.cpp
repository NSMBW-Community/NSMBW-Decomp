#include <types.h>
#include <game/framework/f_arc_load.hpp>

void fArcLoad_c::arcListDelete() {
    if (mpArcList == nullptr || mStatus == STATUS_UNLOADED) {
        return;
    }

    mCount--;
    while (mCount >= 0) {
        deleteArc(mpArcList[mCount]);
        mCount--;
    }
}

int fArcLoad_c::arcListLoad(int loadMany) {
    if (mpArcList == nullptr || mStatus == STATUS_LOADED) {
        return STATUS_LOADED;
    }

    while (mpArcList[mCount] != nullptr) {
        mStatus = loadArc(mpArcList[mCount]);
        if (mStatus != STATUS_LOADED) break;
        mCount++;
        mUnknown = 0;
        if (loadMany == 0) break;
    }

    return mStatus;
}

bool fArcLoad_c::checkDelete() {
    if (mStatus == STATUS_LOADING) {
        mStatus = arcListLoad(0);
    }

    if (mStatus == STATUS_LOADING) {
        return false;
    } else {
        return true;
    }
}
