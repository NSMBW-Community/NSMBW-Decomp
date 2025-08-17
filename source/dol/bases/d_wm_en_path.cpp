#include <game/bases/d_wm_en_path.hpp>
#include <lib/MSL_C/string.h>

dWmEnPath_c::dWmEnPath_c() : mpCurrentPoint(nullptr) {
    for (int i = 0; i < ARRAY_SIZE(mPoints); i++) {
        mPoints[i].mpName = nullptr;
        mPoints[i].mPointIndex = -1;
        mPoints[i].mIndex = -1;
        mPoints[i].mpPrev = nullptr;
        mPoints[i].mpNext = nullptr;
    }
    mAdvancePoint = true;
}

dWmEnPath_c::~dWmEnPath_c() {}

bool dWmEnPath_c::init(const char **names, int count, dWmConnect_c *connect, bool cyclic, int dir) {
    for (int i = 0; i < count; i++) {
        mPoints[i].mpName = names[i];
        mPoints[i].mIndex = i;
        if (i == 0) {
            if (cyclic) {
                mPoints[i].mpPrev = &mPoints[count - 1];
            } else {
                mPoints[i].mpPrev = nullptr;
            }
        } else {
            mPoints[i].mpPrev = &mPoints[i - 1];
        }
        if (i == count - 1) {
            if (cyclic) {
                mPoints[i].mpNext = &mPoints[0];
            } else {
                mPoints[i].mpNext = nullptr;
            }
        } else {
            mPoints[i].mpNext = &mPoints[i + 1];
        }
        int pointIdx = 0;
        while (true) {
            if (strcmp(names[i], connect->GetPointFromIndex(pointIdx)->name) == 0) {
                mPoints[i].mPointIndex = pointIdx;
                break;
            }
            pointIdx++;
        }
    }
    SetStartPoint(0);
    mDir1 = dir;
    mDir2 = dir;
    return true;
}

void dWmEnPath_c::SetStartPoint(int i) {
    mpCurrentPoint = &mPoints[i];
}

int dWmEnPath_c::GetNextPointIdx() {
    return GetNextPointInfo(false)->mPointIndex;
}

int dWmEnPath_c::GetPathPointNo(const char *name) {
    for (int i = 0; i < (int) ARRAY_SIZE(mPoints); i++) {
        if (strcmp(mPoints[i].mpName, name) == 0) {
            return i;
        }
        if (mPoints[i].mpNext == nullptr) {
            break;
        }
    }
    return -1;
}

dWmPathPoint_s *dWmEnPath_c::GetNextPointInfo(bool updateDirection) {
    if (!mAdvancePoint) {
        return mpCurrentPoint;
    }
    dWmPathPoint_s *res = nullptr;
    if (mDir1 == 0) {
        res = mpCurrentPoint->mpNext;
        if (res == nullptr) {
            res = mpCurrentPoint->mpPrev;
            if (updateDirection) {
                mDir1 = 1;
            }
        }
    } else if (mDir1 == 1) {
        res = mpCurrentPoint->mpPrev;
        if (res == nullptr) {
            res = mpCurrentPoint->mpNext;
            if (updateDirection) {
                mDir1 = 0;
            }
        }
    }
    return res;
}

void dWmEnPath_c::UpdatePoint() {
    mpCurrentPoint = GetNextPointInfo(true);
}

bool dWmEnPath_c::isPointIn(int pointIndex) {
    for (int i = 0; i < ARRAY_SIZE(mPoints); i++) {
        if (mPoints[i].mPointIndex == pointIndex) {
            return true;
        }
    }
    return false;
}
