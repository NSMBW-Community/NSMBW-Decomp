#include <game/bases/d_rail.hpp>
#include <game/bases/d_s_stage.hpp>

sRailInfoData *dRail_c::getRailInfoP(u8 id) {
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sRailInfoData *curr = cdFile->mpRails;
    for (u32 i = 0; i < cdFile->mRailCount; i++) {
        if (curr->mID == id) {
            return curr;
        }
        curr++;
    }
    return nullptr;
}

bool dRail_c::set(u8 id, mVec3_c *pos, mVec3_c *speed, float *speedF, u16 startIndex, u8 reverse) {
    mCount = 0;
    mpNodes = nullptr;
    sRailInfoData *railInfo = getRailInfoP(id);
    if (railInfo == nullptr) {
        return false;
    }

    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sRailNodeData *railNode = &cdFile->mpRailNodes[railInfo->mNodeIdx];

    // Check for invalid start index
    if (railInfo->mCount <= startIndex) {
        startIndex = 0;
    }

    float dummy = 0.0f; // [For .sdata2 data order]
    mCount = railInfo->mCount;
    mpPos = pos;
    mpPos->x = railNode[startIndex].mX;
    mpPos->y = -railNode[startIndex].mY;
    mpSpeed = speed;
    mpSpeedF = speedF;
    mpNodes = railNode;
    mIdxCurr = startIndex;
    mIdxNext = startIndex;
    mDelayTimer = railNode->mDelay;
    mReverse = reverse;
    mFlags = railInfo->mFlags;
    mNodeFlags = railNode[startIndex].mFlags;

    calcNextPoint();
    calcTargetPos();
    calcSpeed();
    return true;
}

bool dRail_c::calcNextPoint() {
    bool reachedEnd = false;
    if (mReverse) {
        mIdxNext--;
        if (mIdxNext < 0) {
            if (mFlags & RAIL_FLAG_LOOP) {
                mIdxNext = mCount - 1;
            } else if (mBounce) {
                mReverse ^= 1;
                mIdxNext = 1;
            } else {
                mIdxNext = 0;
                reachedEnd = true;
            }
        }
    } else {
        mIdxNext++;
        if (mIdxNext >= mCount) {
            if (mFlags & RAIL_FLAG_LOOP) {
                mIdxNext = 0;
            } else if (mBounce) {
                mReverse ^= 1;
                mIdxNext = mCount - 2;
            } else {
                mIdxNext = mCount - 1;
                reachedEnd = true;
            }
        }
    }
    return reachedEnd;
}

void dRail_c::calcTargetPos() {
    if (mpNodes == nullptr) {
        return;
    }
    mTargetPos.x = mpNodes[mIdxNext].mX;
    mTargetPos.y = -mpNodes[mIdxNext].mY;
    mAngle = cM::atan2s(mTargetPos.y - mpPos->y, mTargetPos.x - mpPos->x);
    mAngleCalcTimer = 8;
}

void dRail_c::calcSpeed() {
    float speed = mpNodes[mIdxCurr].mSpeed;
    float accel = mpNodes[mIdxCurr].mAccel;
    float prevSpeed = *mpSpeedF;
    float newSpeed;
    if (accel == 0.0f) {
        newSpeed = speed;
    } else {
        if (prevSpeed < speed) {
            newSpeed = prevSpeed + accel;
            if (newSpeed > speed) {
                newSpeed = speed;
            }
        } else {
            newSpeed = prevSpeed;
            if (prevSpeed > speed) {
                newSpeed = prevSpeed - accel;
                if (newSpeed < speed) {
                    newSpeed = speed;
                }
            }
        }
    }

    *mpSpeedF = newSpeed;

    if (mAngleCalcTimer > 0){
        mAngleCalcTimer--;
    } else {
        mAngleCalcTimer = 8;
        mAngle = cM::atan2s(mTargetPos.y - mpPos->y, mTargetPos.x - mpPos->x);
    }

    mpSpeed->x = *mpSpeedF * nw4r::math::CosIdx(mAngle);
    mpSpeed->y = *mpSpeedF * nw4r::math::SinIdx(mAngle);
    mpSpeed->z = 0.0f;
}

bool dRail_c::execute() {
    mNodeFlags = 0;
    if (mpNodes == nullptr) {
        return false;
    }
    if (mFlags & RAIL_FLAG_DISABLED) {
        return false;
    }
    if (mDelayTimer != 0) {
        mDelayTimer--;
        return false;
    }
    mFlags &= ~RAIL_FLAG_DELAYING;

    *mpPos += *mpSpeed;

    bool res = false;
    if (checkArrive()) {
        mNodeFlags = mpNodes[mIdxNext].mFlags;
        if (mNodeFlags & NODE_FLAG_1) {
            mFlags |= RAIL_FLAG_DISABLED | RAIL_FLAG_DELAYING;
        } else {
            mIdxCurr = mIdxNext;
            if (calcNextPoint()) {
                mFlags |= RAIL_FLAG_DISABLED | RAIL_FLAG_DELAYING;
            } else {
                calcTargetPos();
                mDelayTimer = mpNodes[mIdxCurr].mDelay;
                mFlags |= RAIL_FLAG_DELAYING;
            }
        }
        res = true;
    }
    calcSpeed();
    return res;
}

bool dRail_c::checkArrive() {
    float dx = mTargetPos.x - mpPos->x;
    float dy = mTargetPos.y - mpPos->y;
    float squareDist = dx * dx + dy * dy;
    float squareSpeed = *mpSpeedF * *mpSpeedF;
    return squareDist < squareSpeed;
}
