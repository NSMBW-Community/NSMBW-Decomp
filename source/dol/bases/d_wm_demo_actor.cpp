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

void dWmDemoActor_c::setCutEndSpecific(int cutsceneId, bool param2) {
    if (cutsceneId == -1) {
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

void dWmDemoActor_c::InitJumpParam(const mVec3_c &startPos, const mVec3_c &targetPos,
                                   int numFrames, float jumpSpeed, float maxYSpeed) {
    if (startPos.y == targetPos.y) {
        __initJumpParam1(startPos, targetPos, numFrames, jumpSpeed, maxYSpeed);
    } else {
        __initJumpParam2(startPos, targetPos, numFrames, jumpSpeed, maxYSpeed);
    }
}

void dWmDemoActor_c::__initJumpParam1(const mVec3_c &startPos, const mVec3_c &targetPos,
                                      int numFrames, float jumpSpeed, float maxYSpeed) {

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
    mSpeedF = (targetPos - startPos).xzLen() / numFramesF;
    mMaxFallSpeed = maxYSpeed;
    mAccelY = accel;
    mSpeed.y = jumpSpeed;
}

void dWmDemoActor_c::__initJumpParam2(const mVec3_c &startPos, const mVec3_c &targetPos,
                                      int numFrames, float jumpSpeed, float maxYSpeed) {
    // The formula is derived from s = v₀ * t + ½ * a * t², where:
    // s  = targetPos.y - startPos.y
    // t  = numFrames
    // v₀ = jumpSpeed

    // Rearrange for a:
    // ½ * a * t² = s - v₀ * t
    // a = (s - v₀ * t) * 2 / t²
    // a = (v₀ * t - s) * -2 / t² [(not sure why this step was done)]

    float numFramesF = numFrames;
    float accel = ((jumpSpeed * numFramesF - (targetPos.y - startPos.y)) * -2.0f) / (numFramesF * numFramesF);
    mSpeedF = (targetPos - startPos).xzLen() / numFramesF;
    mMaxFallSpeed = maxYSpeed;
    mAccelY = accel;
    mSpeed.y = jumpSpeed;
}

void dWmDemoActor_c::initJumpBase(mVec3_c pos, int numFrames, float jumpSpeed) {
    mTargetPos = pos;
    InitJumpParam(mPos, mTargetPos, numFrames, jumpSpeed, -50.0f);
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
    mVec3_c direction = dir;
    direction.normalize();
    if (EGG::Mathf::abs(direction.x) < 0.1f && EGG::Mathf::abs(direction.z) < 0.1f) {
        direction = mVec3_c(0.0f, 0.0f, -1.0f);
    }
    s16 ang = cM::atan2s(direction.x, direction.z);
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

bool dWmDemoActor_c::checkArriveTargetXYZ(const mVec3_c &startPos, const mVec3_c &targetPos) {
    float dist = startPos.distTo(targetPos);
    float distToLast = startPos.distTo(mLastPos);
    float distToCurr = startPos.distTo(mPos);
    if (distToLast - dist < 0 && distToCurr - dist >= 0) {
        return true;
    }
    return false;
}

bool dWmDemoActor_c::checkArriveTargetXZ(const mVec3_c &startPos, const mVec3_c &targetPos) {
    float dist = (targetPos - startPos).xzLen();
    float distToLast = (mLastPos - startPos).xzLen();
    float distToCurr = (mPos - startPos).xzLen();
    if (distToLast - dist < 0 && distToCurr - dist >= 0) {
        return true;
    }
    return false;
}

void dWmDemoActor_c::CreateShadowModel(const char *arc, const char *path, const char *mdlName, bool param4) {
    mHeapAllocator.createHeapRestoreCurrent(-1, mHeap::g_gameHeaps[0], nullptr, 0x20, 0);

    nw4r::g3d::ResFile resFile = dResMng_c::m_instance->mRes.getRes(arc, path);
    mModel.create(resFile.GetResMdl(mdlName), &mHeapAllocator, 0x20, 1, 0);

    mSvMdl = new dWmSVMdl_c();
    mSvMdl->create(&mHeapAllocator, mModel);

    mHeapAllocator.adjustFrmHeapRestoreCurrent();
}

void dWmDemoActor_c::CalcShadow(float yOffs, float scaleX, float scaleY, float scaleZ) {
    mVec3_c shadowPos = mPos;
    mAng3_c shadowRot = mAng3_c::onlyY(mAngle.y);
    shadowPos.y = mPos.y + yOffs;
    mMatrix.trans(shadowPos);
    mMatrix.ZXYrotM(shadowRot);
    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(scaleX, scaleY, scaleZ);
    mModel.calc(false);
}

void dWmDemoActor_c::CalcShadow(float yOffs, float scale) {
    CalcShadow(yOffs, scale, 1.0f, scale);
}

void dWmDemoActor_c::DrawShadow(bool param1) {
    if (mSvMdl != nullptr) {
        mSvMdl->entry();
    }
}

void dWmDemoActor_c::_initDemoJumpBaseCore(const mVec3_c &pos, int delay, int frames, float jumpSpeed,
                                           float scaleStart, float scaleTarget, const short &angY) {
    mScaleCurr = scaleStart;
    mScale.x = scaleStart * 1.0;
    mScale.y = scaleStart * 1.0;
    mScale.z = scaleStart * 1.0;
    mScaleDelta = (scaleTarget - scaleStart) / frames;
    mScaleTarget = scaleTarget;
    mScaleDelay = delay;
    initJumpBase(pos, frames, jumpSpeed);
    mAngle.y = angY;
}

void dWmDemoActor_c::_initDemoJumpBase(const mVec3_c &pos, int delay, int frames, float jumpSpeed,
                                       float scaleStart, float scaleTarget, const mVec3_c &dir) {
    short ang = cM::atan2s(dir.x, dir.z);
    _initDemoJumpBaseCore(pos, delay, frames, jumpSpeed, scaleStart, scaleTarget, ang);
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

float dWmDemoActor_c::GetBgPosY(const mVec3_c &startPos, const mVec3_c &targetPos, int directionType) {
    float yPos = 0.0f;
    switch (directionType) {
        case dWmLib::DIR3D_FRONT:
        case dWmLib::DIR3D_BACK: {
            float diffz = EGG::Mathf::abs(startPos.z - targetPos.z);
            float diffpz = EGG::Mathf::abs(mPos.z - startPos.z);
            float ratio = diffpz / diffz;

            float delta = targetPos.y - startPos.y;
            if (EGG::Mathf::abs(delta) < 5.0f) {
                delta = 0.0f;
            }
            yPos = delta * ratio + startPos.y;
            break;
        }
        case dWmLib::DIR3D_LEFT:
        case dWmLib::DIR3D_RIGHT: {
            float diffx = EGG::Mathf::abs(startPos.x - targetPos.x);
            float diffpx = EGG::Mathf::abs(mPos.x - startPos.x);
            float ratio = diffpx / diffx;

            float delta = targetPos.y - startPos.y;
            if (EGG::Mathf::abs(delta) < 5.0f) {
                delta = 0.0f;
            }
            yPos = delta * ratio + startPos.y;
            break;
        }
        case dWmLib::DIR3D_UP:
        case dWmLib::DIR3D_DOWN:
            yPos = targetPos.y;
    }
    return yPos;
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

void dWmDemoActor_c::adjustHeightBase(const mVec3_c &startPos, const mVec3_c &targetPos, int directionType) {
    if (directionType < 0) {
        directionType = dWmLib::getPointDir(startPos, targetPos);
    }
    CsSPosSimple(directionType, GetBgPosY(startPos, targetPos, directionType));
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
