#include <game/bases/d_wm_demo_actor.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_res.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/cLib/c_lib.hpp>
#include <game/cLib/c_math.hpp>
#include <game/mLib/m_heap.hpp>
#include <lib/nw4r/g3d/res_file.hpp>

dWmDemoActor_c::dWmDemoActor_c() {}

dBaseActor_c *dWmDemoActor_c::GetChildDemoActor(dBaseActor_c *prev, dWmDemoActor_c *&next) {
    dBaseActor_c *actor = (dBaseActor_c *) fManager_c::searchBaseByGroupType(fBase_c::ACTOR, prev);
    if (actor == nullptr) {
        next = nullptr;
        return nullptr;
    }
    switch (actor->GetActorType()) {
        case dBaseActor_c::ACTOR_MAP_DEMO:
        case dBaseActor_c::ACTOR_MAP_OBJECT:
        case dBaseActor_c::ACTOR_MAP_ENEMY:
        case dBaseActor_c::ACTOR_MAP_PLAYER:
            next = (dWmDemoActor_c *) actor;
            break;
        default:
            next = nullptr;
            break;
    }
    return actor;
}

void dWmDemoActor_c::setCutEndSpecific(int csId, bool p2) {
    if (csId == -1) {
        return;
    }
    dWmDemoActor_c::setCutEnd();
}

bool dWmDemoActor_c::isStaff() {
    dWmDemoActor_c *a1 = dCsSeqMng_c::ms_instance->m_1ac;
    dWmDemoActor_c *a2 = dCsSeqMng_c::ms_instance->m_1b0;
    if (a1 == this || a2 == this) {
        return true;
    }
    return false;
}

void dWmDemoActor_c::InitJumpParam(const mVec3_c &start, const mVec3_c &end, int numFrames, float jumpSpeed, float maxYSpeed) {
    if (start.y == end.y) {
        __initJumpParam1(start, end, numFrames, jumpSpeed, maxYSpeed);
    } else {
        __initJumpParam2(start, end, numFrames, jumpSpeed, maxYSpeed);
    }
}

void dWmDemoActor_c::__initJumpParam1(const mVec3_c &start, const mVec3_c &end, int numFrames, float jumpSpeed, float maxYSpeed) {
    // The formula is derived from s = v₀ * t + ½ * a * t².
    // In this case, we land at the same height we started, so we have
    // 0 = v₀ * t + ½ * a * t², where
    // t  = numFrames
    // v₀ = jumpSpeed

    // Rearrange for a:
    // ½ * a * t² = -v₀ * t
    // a = (2 * -v₀ * t) / t²
    // a = (2 * -v₀) / t
    // a = -(2 * v₀) / t

    float numFramesF = numFrames;
    float accel = -(2.0f * jumpSpeed) / numFramesF;
    mSpeedF = (end - start).xzLen() / numFramesF;
    mMaxFallSpeed = maxYSpeed;
    mAccelY = accel;
    mSpeed.y = jumpSpeed;
}

void dWmDemoActor_c::__initJumpParam2(const mVec3_c &start, const mVec3_c &end, int numFrames, float jumpSpeed, float maxYSpeed) {
    // The formula is derived from s = v₀ * t + ½ * a * t², where:
    // s  = end.y - start.y
    // t  = numFrames
    // v₀ = jumpSpeed

    // Rearrange for a:
    // ½ * a * t² = s - v₀ * t
    // a = (s - v₀ * t) * 2 / t²
    // a = (v₀ * t - s) * -2 / t² [(not sure why this step was done)]

    float numFramesF = numFrames;
    float accel = ((jumpSpeed * numFramesF - (end.y - start.y)) * -2.0f) / (numFramesF * numFramesF);
    mSpeedF = (end - start).xzLen() / numFramesF;
    mMaxFallSpeed = maxYSpeed;
    mAccelY = accel;
    mSpeed.y = jumpSpeed;
}

void dWmDemoActor_c::initJumpBase(mVec3_c pos, int numFrames, float jumpSpeed) {
    mTargetPos = pos;
    InitJumpParam(mPos, mTargetPos, numFrames, jumpSpeed, -50.0);
    s16 ang = cLib::targetAngleY(mPos, mTargetPos);
    mAngle.y = ang;
    mAngle3D.y = ang;
}

bool dWmDemoActor_c::procJumpBase() {
    calcSpeed();
    posMove();
    if (mLastPos.y >= mTargetPos.y && mPos.y <= mTargetPos.y) {
        mPos = mTargetPos;
        return true;
    }
    return false;
}

void dWmDemoActor_c::setDirection(const mVec3_c &dir) {
    mVec3_c copy = dir;
    copy.normalize();
    if (EGG::Mathf::abs(copy.x) < 0.1f && EGG::Mathf::abs(copy.z) < 0.1f) {
        copy = mVec3_c(0.0f, 0.0f, -1.0f);
    }
    s16 ang = cM::atan2s(copy.x, copy.z);
    mAngle.y = ang;
    mAngle3D.y = ang;
}

void dWmDemoActor_c::rotDirectionY(short angle, bool is3D) {
    mAngle.y += angle;
    if (is3D) {
        mAngle3D.y = mAngle.y;
    }
}

void dWmDemoActor_c::rotDirectionX(short angle, bool is3D) {
    mAngle.x += angle;
    if (is3D) {
        mAngle3D.x = mAngle.x;
    }
}

bool dWmDemoActor_c::checkArriveTargetXYZ(const mVec3_c &start, const mVec3_c &target) {
    float dist = start.distTo(target);
    float distToLast = start.distTo(mLastPos);
    float distToCurr = start.distTo(mPos);
    if (distToLast - dist < 0 && distToCurr - dist >= 0) {
        return true;
    }
    return false;
}

bool dWmDemoActor_c::checkArriveTargetXZ(const mVec3_c &start, const mVec3_c &target) {
    float dist = (target - start).xzLen();
    float distToLast = (mLastPos - start).xzLen();
    float distToCurr = (mPos - start).xzLen();
    if (distToLast - dist < 0 && distToCurr - dist >= 0) {
        return true;
    }
    return false;
}

void dWmDemoActor_c::CreateShadowModel(const char *arc, const char *path, const char *mdlName, bool b) {
    mHeapAllocator.createHeapRestoreCurrent(-1, mHeap::g_gameHeaps[0], nullptr, 0x20, 0);

    nw4r::g3d::ResFile resFile = dResMng_c::m_instance->mRes.getRes(arc, path);
    mModel.create(resFile.GetResMdl(mdlName), &mHeapAllocator, 0x20, 1, 0);

    mSvMdl = new dWmSVMdl_c();
    mSvMdl->create(&mHeapAllocator, mModel);

    mHeapAllocator.adjustFrmHeapRestoreCurrent();
}

void dWmDemoActor_c::CalcShadow(float yOffs, float scaleX, float scaleY, float scaleZ) {
    mVec3_c tVec = mPos;
    mAng3_c a = mAng3_c::onlyY(mAngle.y);
    tVec.y = mPos.y + yOffs;
    mMatrix.trans(tVec);
    mMatrix.ZXYrotM(a);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(scaleX, scaleY, scaleZ);
    mModel.calc(false);
}

void dWmDemoActor_c::CalcShadow(float yOffs, float scale) {
    CalcShadow(yOffs, scale, 1.0f, scale);
}

void dWmDemoActor_c::DrawShadow(bool b) {
    if (mSvMdl != nullptr) {
        mSvMdl->entry();
    }
}

void dWmDemoActor_c::_initDemoJumpBaseCore(const mVec3_c &pos, int delay, int frames, float jumpSpeed, float start, float target, const short &angY) {
    mScaleCurr = start;
    mScale.x = start * 1.0;
    mScale.y = start * 1.0;
    mScale.z = start * 1.0;
    mScaleDelta = (target - start) / frames;
    mScaleTarget = target;
    mScaleDelay = delay;
    initJumpBase(pos, frames, jumpSpeed);
    mAngle.y = angY;
}

void dWmDemoActor_c::_initDemoJumpBase(const mVec3_c &pos, int delay, int frames, float jumpSpeed, float start, float target, const mVec3_c &dir) {
    short ang = cM::atan2s(dir.x, dir.z);
    _initDemoJumpBaseCore(pos, delay, frames, jumpSpeed, start, target, ang);
}

bool dWmDemoActor_c::_procDemoJumpBase() {
    if (mScaleDelay > 0) {
        mScaleDelay--;
    } else {
        mScaleCurr += mScaleDelta;
        if (mScaleDelta > 0.0f) {
            if (mScaleCurr < mScaleTarget) {
                mScale.set(mScaleCurr, mScaleCurr, mScaleCurr);
            } else {
                mScaleCurr = mScaleTarget;
                mScale.set(mScaleTarget, mScaleTarget, mScaleTarget);
            }
        } else {
            if (mScaleCurr > mScaleTarget) {
                mScale.set(mScaleCurr, mScaleCurr, mScaleCurr);
            } else {
                mScaleCurr = mScaleTarget;
                mScale.set(mScaleTarget, mScaleTarget, mScaleTarget);
            }
        }
        return procJumpBase();
    }
    return false;
}

float dWmDemoActor_c::GetBgPosY(const mVec3_c &start, const mVec3_c &target, int directionType) {
    float res = 0.0f;
    switch (directionType) {
        case dWmLib::DIR3D_FRONT:
        case dWmLib::DIR3D_BACK: {
            float diffz = EGG::Mathf::abs(start.z - target.z);
            float diffpz = EGG::Mathf::abs(mPos.z - start.z);
            float ratio = diffpz / diffz;

            float scale = target.y - start.y;
            if (EGG::Mathf::abs(scale) < 5.0f) {
                scale = 0.0f;
            }
            res = scale * ratio + start.y;
            break;
        }
        case dWmLib::DIR3D_LEFT:
        case dWmLib::DIR3D_RIGHT: {
            float diffx = EGG::Mathf::abs(start.x - target.x);
            float diffpx = EGG::Mathf::abs(mPos.x - start.x);
            float ratio = diffpx / diffx;

            float scale = target.y - start.y;
            if (EGG::Mathf::abs(scale) < 5.0f) {
                scale = 0.0f;
            }
            res = scale * ratio + start.y;
            break;
        }
        case dWmLib::DIR3D_UP:
        case dWmLib::DIR3D_DOWN:
            res = target.y;
    }
    return res;
}

void dWmDemoActor_c::CsSPosSimple(int directionType, float yTarget) {
    switch (directionType) {
        case dWmLib::DIR3D_FRONT:
        case dWmLib::DIR3D_BACK:
        case dWmLib::DIR3D_LEFT:
        case dWmLib::DIR3D_RIGHT:
            if (mSpeed.y < 0.0f && mPos.y < yTarget) {
                mPos.y = yTarget;
            }
            break;
        case dWmLib::DIR3D_UP:
            if (mPos.y > yTarget) {
                mPos.y = yTarget;
            }
            break;
        case dWmLib::DIR3D_DOWN:
            if (mPos.y < yTarget) {
                mPos.y = yTarget;
            }
            break;
    }
}

void dWmDemoActor_c::clearSpeedAll() {
    mSpeedF = 0.0f;
    mAccelY = 0.0f;
    mSpeed.y = 0.0f;
    mMaxFallSpeed = 0.0f;
}

void dWmDemoActor_c::adjustHeightBase(const mVec3_c &start, const mVec3_c &target, int directionType) {
    if (directionType < 0) {
        directionType = dWmLib::getPointDir(start, target);
    }
    CsSPosSimple(directionType, GetBgPosY(start, target, directionType));
}

bool dWmDemoActor_c::isCutscenePlaying(int *csList, int csCount) {
    int cutName = dCsSeqMng_c::ms_instance->GetCutName();
    bool found = false;
    for (int i = 0; i < csCount; i++) {
        if (csList[i] == cutName) {
            found = true;
            break;
        }
    }
    return found;
}
