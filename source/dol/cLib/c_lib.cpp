#include <game/cLib/c_lib.hpp>
#include <lib/egg/math/eggMath.h>
#include <lib/revolution/MTX/vec.h>
#include <string.h>

inline float calcDistance(const mVec3_c &a, const mVec3_c &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return EGG::Mathf::sqrt(dx * dx + dy * dy + dz * dz);
}

namespace cLib {

void memSet(void *dst, int val, ulong size) {
    memset(dst, val, size);
}

float addCalcPos(mVec3_c *currPos, const mVec3_c &targetPos, float ratio, float maxStep, float minStep) {
    if (*currPos != targetPos) {
        mVec3_c stepVector;
        stepVector = *currPos - targetPos;
        float distance = PSVECMag(stepVector);
        if (distance < minStep) {
            *currPos = targetPos;
        } else {
            stepVector *= ratio;
            float step = distance * ratio;
            if (!isZero(step)) {
                if (step > maxStep) {
                    stepVector *= maxStep / step;
                } else if (step < minStep) {
                    stepVector *= minStep / step;
                }
                *currPos -= stepVector;
            } else {
                *currPos = targetPos;
            }
        }
    }

    return calcDistance(*currPos, targetPos);
}

bool chasePos(mVec3_c *currentPos, const mVec3_c &targetPos, float step) {
    if (step) {
        mVec3_c offset = *currentPos - targetPos;
        float distance = PSVECMag(offset);
        if (isZero(distance) || distance <= step) {
            *currentPos = targetPos;
            return true;
        }
        *currentPos -= (step / distance) * offset;
    } else if (*currentPos == targetPos) {
        return true;
    }
    return false;
}

s16 targetAngleY(const mVec3_c &vec1, const mVec3_c &vec2) {
    return cM::atan2s(vec2.x - vec1.x, vec2.z - vec1.z);
}

s16 targetAngleX(const mVec3_c &vec1, const mVec3_c &vec2) {
    mVec3_c diff = vec2 - vec1;
    return cM::atan2s(diff.y, diff.xzLen());
}

} // namespace cLib
