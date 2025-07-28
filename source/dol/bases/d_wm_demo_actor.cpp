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

dWmDemoActor_c::~dWmDemoActor_c() {}


dWmDemoActor_c *dWmDemoActor_c::FUN_800f6130(dWmDemoActor_c **outActor) {
    dWmDemoActor_c *actor = (dWmDemoActor_c *) fManager_c::searchBaseByGroupType(fBase_c::ACTOR, this);
    if (actor == nullptr) {
        *outActor = nullptr;
        return nullptr;
    }
    int type = actor->GetActorType();
    if (type == 1 || type == 2 || type == 3 || type == 4) {
        *outActor = actor;
    } else {
        *outActor = nullptr;
    }
    return actor;
}

void dWmDemoActor_c::setCutEndSpecific(int csId, bool p2) {
    if (csId == -1) {
        return;
    }
    mIsCutEnd = true;
}

void dWmDemoActor_c::setCutEnd() {
    mIsCutEnd = true;
}

bool dWmDemoActor_c::isStaff() {
    dWmDemoActor_c *a1 = dCsSeqMng_c::ms_instance->m_1ac;
    dWmDemoActor_c *a2 = dCsSeqMng_c::ms_instance->m_1b0;
    if (a1 == this || a2 == this) {
        return true;
    }
    return false;
}

void dWmDemoActor_c::InitJumpParam(const mVec3_c &v1, const mVec3_c &v2, int p3, float p4, float p5) {
  if (v1.y == v2.y) {
    __initJumpParam1(v1, v2, p3, p4, p5);
  } else {
    __initJumpParam2(v1, v2, p3, p4, p5);
  }
}

void dWmDemoActor_c::__initJumpParam1(const mVec3_c &v1, const mVec3_c &v2, int p3, float p4, float p5) {
    // TODO: Not fully matching
    float f2 = -(p4 * 2.0f) / p5;
    float f = p3;
    // float len = (v2 - v1).xzLen();
    mSpeedF = (v2 - v1).xzLen() / f;
    mMaxFallSpeed = p5;
    mAccelY = f2;
    mSpeed.y = p4;
}

void dWmDemoActor_c::__initJumpParam2(const mVec3_c &v1, const mVec3_c &v2, int p3, float p4, float p5) {
    // TODO
    float f = -2.0f;
}

void dWmDemoActor_c::initJumpBase(mVec3_c v, int i, float f) {
    m_30 = v;
    InitJumpParam(mPos, m_30, i, f, -50.0);
    s16 ang = cLib::targetAngleY(mPos, m_30);
    mAngle.y = ang;
    mAngle3D.y = ang;
}

bool dWmDemoActor_c::procJumpBase() {
    calcSpeed();
    posMove();
    if (mLastPos.y >= m_30.y && mPos.y <= m_30.y) {
        mPos = m_30;
        return true;
    }
    return false;
}

void dWmDemoActor_c::setDirection(const mVec3_c &v) {
    mVec3_c copy = v;
    copy.normalize();
    if (EGG::Mathf::abs(copy.x) < 0.1f && EGG::Mathf::abs(copy.z) < 0.1f) {
        copy = mVec3_c(0.0f, 0.0f, -1.0f);
    }
    s16 ang = cM::atan2s(copy.x, copy.z);
    mAngle.y = ang;
    mAngle3D.y = ang;
}

void dWmDemoActor_c::rotDirectionY(short s, bool b) {
    mAngle.y += s;
    if (b) {
        mAngle3D.y = mAngle.y;
    }
}

void dWmDemoActor_c::rotDirectionX(short s, bool b) {
    mAngle.x += s;
    if (b) {
        mAngle3D.x = mAngle.x;
    }
}

bool dWmDemoActor_c::checkArriveTargetXYZ(const mVec3_c &v1, const mVec3_c &v2) {
    float dist1 = v1.distTo(v2);
    float dist2 = v1.distTo(mLastPos);
    float dist3 = v1.distTo(mPos);
    if (dist2 - dist1 < 0 && dist3 - dist1 >= 0) {
        return true;
    }
    return false;
}

bool dWmDemoActor_c::checkArriveTargetXZ(const mVec3_c &v1, const mVec3_c &v2) {
    float dist1 = (v2 - v1).xzLen();
    float dist2 = (mLastPos - v1).xzLen();
    float dist3 = (mPos - v1).xzLen();
    if (dist2 - dist1 < 0 && dist3 - dist1 >= 0) {
        return true;
    }
    return false;
}

void dWmDemoActor_c::FUN_800f6820(const char *arc, const char *path, const char *mdlName) {
    mHeapAllocator.createHeapRestoreCurrent(-1, mHeap::g_gameHeaps[0], nullptr, 0x20, 0);

    nw4r::g3d::ResFile resFile = dResMng_c::m_instance->mRes.getRes(arc, path);
    mModel.create(resFile.GetResMdl(mdlName), &mHeapAllocator, 0x20, 1, 0);

    mSvMdl = new dWmSVMdl_c();
    mSvMdl->vf20(&mHeapAllocator, &mModel);

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

void dWmDemoActor_c::_initDemoJumpBaseCore(const mVec3_c &pos, int i, int i2, float f1, float scale, float f3, const short &angY) {
    m_3c = scale;
    mScale.x = scale * 1.0;
    mScale.y = scale * 1.0;
    mScale.z = scale * 1.0;
    m_40 = (f3 - scale) / i2;
    m_44 = f3;
    m_48 = i;
    initJumpBase(pos, i2, f1);
    mAngle.y = angY;
}

void dWmDemoActor_c::_initDemoJumpBase(const mVec3_c &pos, int i, int i2, float f1, float scale, float f3, const mVec3_c &vec) {
    short ang = cM::atan2s(vec.x, vec.z);
    _initDemoJumpBaseCore(pos, i, i2, f1, scale, f3, ang);
}

bool dWmDemoActor_c::_procDemoJumpBase() {
    if (m_48 > 0) {
        m_48--;
    } else {
        m_3c += m_40;
        if (m_40 > 0) {
            if (m_3c < m_44) {
                mScale.set(m_3c, m_3c, m_3c);
            } else {
                m_3c = m_44;
                mScale.set(m_44, m_44, m_44);
            }
        } else {
            if (m_3c > m_44) {
                mScale.set(m_3c, m_3c, m_3c);
            } else {
                m_3c = m_44;
                mScale.set(m_44, m_44, m_44);
            }
        }
        return procJumpBase();
    }
    return false;
}

float dWmDemoActor_c::GetBgPosY(const mVec3_c &v1, const mVec3_c &v2, int i) {
    float res = 0.0f;
    switch (i) {
        case 2:
        case 3: {
            float diffz = EGG::Mathf::abs(v1.z - v2.z);
            float diffpz = EGG::Mathf::abs(mPos.z - v1.z);
            float ratio = diffpz / diffz;

            float scale = v2.y - v1.y;
            if (EGG::Mathf::abs(scale) < 5.0f) {
                scale = 0.0f;
            }
            res = scale * ratio + v1.y;
            break;
        }
        case 4:
        case 5: {
            float diffx = EGG::Mathf::abs(v1.x - v2.x);
            float diffpx = EGG::Mathf::abs(mPos.x - v1.x);
            float ratio = diffpx / diffx;

            float scale = v2.y - v1.y;
            if (EGG::Mathf::abs(scale) < 5.0f) {
                scale = 0.0f;
            }
            res = scale * ratio + v1.y;
            break;
        }
        case 0:
        case 1:
            res = v2.y;
    }
    return res;
}

void dWmDemoActor_c::CsSPosSimple(int i, float y) {
    switch (i) {
        case 2:
        case 3:
        case 4:
        case 5:
            if (mSpeed.y < 0.0f && mPos.y < y) {
                mPos.y = y;
            }
            break;
        case 0:
            if (mPos.y > y) {
                mPos.y = y;
            }
            break;
        case 1:
            if (mPos.y < y) {
                mPos.y = y;
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

void dWmDemoActor_c::adjustHeightBase(const mVec3_c &v1, const mVec3_c &v2, int i) {
    if (i < 0) {
        i = dWmLib::getPointDir(v1, v2);
    }
    CsSPosSimple(i, GetBgPosY(v1, v2, i));
}

bool dWmDemoActor_c::FUN_800f6e70(int *csList, int csCount) {
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

void dWmDemoActor_c::clearCutEnd() {
    mIsCutEnd = false;
}

bool dWmDemoActor_c::checkCutEnd() {
    return mIsCutEnd;
}

int dWmDemoActor_c::GetActorType() {
    return ACTOR_MAP_DEMO;
}

int dWmDemoActor_c::doDelete() {
    return SUCCEEDED;
}

int dWmDemoActor_c::draw() {
    return SUCCEEDED;
}

int dWmDemoActor_c::create() {
    return SUCCEEDED;
}
