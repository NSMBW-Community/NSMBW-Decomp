#include <game/mLib/m_3d.hpp>
#include <lib/nw4r/ut/inlines.hpp>

m3d::mdl_c::mdlCallback_c::mdlCallback_c() :
    mNodeCount(0), mpNodeResults(nullptr), mpCallback(nullptr), mpAllocator(nullptr) {}

m3d::mdl_c::mdlCallback_c::~mdlCallback_c() {}

void m3d::mdl_c::mdlCallback_c::ExecCallbackA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw) {
    u16 nodeID = cw->mNodeID;
    nw4r::g3d::ChrAnmResult *result = &mpNodeResults[nodeID];

    if (mCalcRatio.isActive() && !mCalcRatio.isEnd()) {
        if (!mCalcRatio.isBlending()) {
            *anmRes = *result;
        } else {
            float slerpParam = mCalcRatio.getSlerpParam();
            float scaleFrom = mCalcRatio.getScaleFrom();
            float scaleTo = mCalcRatio.getScaleTo();

            u32 flags = anmRes->mFlags;

            if ((flags & nw4r::g3d::ChrAnmResult::FLAG_SCALE_ONE) == 0) {
                anmRes->mScale.x = anmRes->mScale.x * scaleTo + result->mScale.x * scaleFrom;
                anmRes->mScale.y = anmRes->mScale.y * scaleTo + result->mScale.y * scaleFrom;
                anmRes->mScale.z = anmRes->mScale.z * scaleTo + result->mScale.z * scaleFrom;
            } else {
                anmRes->mScale.x = scaleTo + result->mScale.x * scaleFrom;
                anmRes->mScale.y = scaleTo + result->mScale.y * scaleFrom;
                anmRes->mScale.z = scaleTo + result->mScale.z * scaleFrom;
            }

            QUAT p, q;
            C_QUATMtx(&p, &result->mMtx.mtx);
            if ((flags & nw4r::g3d::ChrAnmResult::FLAG_ROTATE_ZERO) == 0) {
                C_QUATMtx(&q, &anmRes->mMtx.mtx);
            } else {
                q.x = 0;
                q.y = 0;
                q.z = 0;
                q.w = 1;
            }

            C_QUATSlerp(&p, &q, &p, slerpParam);

            nw4r::math::VEC3 cpy;
            cpy.x = anmRes->mMtx.mData[0][3];
            cpy.y = anmRes->mMtx.mData[1][3];
            cpy.z = anmRes->mMtx.mData[2][3];
            PSMTXQuat(&anmRes->mMtx.mtx, &p);
            anmRes->mMtx.mData[0][3] = cpy.x;
            anmRes->mMtx.mData[1][3] = cpy.y;
            anmRes->mMtx.mData[2][3] = cpy.z;

            if ((flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO) == 0) {
                anmRes->mMtx.mData[0][3] = cpy.x * scaleTo;
                anmRes->mMtx.mData[1][3] = cpy.y * scaleTo;
                anmRes->mMtx.mData[2][3] = cpy.z * scaleTo;
            }

            anmRes->mMtx.mData[0][3] += result->mMtx.mData[0][3] * scaleFrom;
            anmRes->mMtx.mData[1][3] += result->mMtx.mData[1][3] * scaleFrom;
            anmRes->mMtx.mData[2][3] += result->mMtx.mData[2][3] * scaleFrom;

            anmRes->mFlags &= ~(nw4r::g3d::ChrAnmResult::FLAG_RAW_ROTATE_VALID |
                                nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO |
                                nw4r::g3d::ChrAnmResult::FLAG_ROTATE_ZERO |
                                nw4r::g3d::ChrAnmResult::FLAG_SCALE_ONE);

            *result = *anmRes;
        }
    } else {
        *result = *anmRes;
    }

    if (mpCallback != nullptr) {
        mpCallback->timingA(nodeID, anmRes, resMdl);
    }
}

void m3d::mdl_c::mdlCallback_c::ExecCallbackB(nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw) {
    u16 nodeID = cw->mNodeID;
    if (mpCallback != nullptr) {
        mpCallback->timingB(nodeID, manip, resMdl);
    }

    u32 nodeCount = resMdl.GetResNodeNumEntries();
    cw->mNodeID = (nodeID + 1) % nodeCount;
}

void m3d::mdl_c::mdlCallback_c::ExecCallbackC(nw4r::math::MTX34 *mtx, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw) {
    if (mpCallback != nullptr) {
        mpCallback->timingC(mtx, resMdl);
    }
    mCalcRatio.offUpdate();
}

bool m3d::mdl_c::mdlCallback_c::create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, size_t *pSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t size = 0;
    if (pSize == nullptr) {
        pSize = &size;
    }

    mNodeCount = resMdl.GetResNodeNumEntries();
    size_t resultSize = mNodeCount * sizeof(nw4r::g3d::ChrAnmResult);

    mpNodeResults = (nw4r::g3d::ChrAnmResult *) MEMAllocFromAllocator(allocator, resultSize);
    if (mpNodeResults == nullptr) {
        return false;
    }

    *pSize = nw4r::ut::RoundUp(resultSize + nw4r::ut::RoundUp(*pSize, 4), 4);

    nw4r::g3d::ChrAnmResult *curr = mpNodeResults;
    for (int i = 0; i < mNodeCount; i++) {
        curr->mScale.x = 1.0f;
        curr->mScale.y = 1.0f;
        curr->mScale.z = 1.0f;
        PSMTXIdentity(&curr->mMtx.mtx);
        curr++;
    }

    mpAllocator = allocator;
    return true;
}

void m3d::mdl_c::mdlCallback_c::remove() {
    mCalcRatio.remove();
    if (mpNodeResults != nullptr) {
        mpAllocator->free(mpNodeResults);
    }
    mpNodeResults = nullptr;
    mpAllocator = nullptr;
}

void m3d::mdl_c::mdlCallback_c::setBlendFrame(float blendFrame) {
    mCalcRatio.set(blendFrame);
}

void m3d::mdl_c::mdlCallback_c::calcBlend() {
    if (!mCalcRatio.isEnd()) {
        mCalcRatio.calc();
    }
}

m3d::mdl_c::mdl_c() {}
m3d::mdl_c::~mdl_c() {}

bool m3d::mdl_c::create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, ulong bufferOption, int viewCount, size_t *pSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t *pMdlSize = nullptr;
    size_t *pMdlCallbackSize = nullptr;
    size_t mdlSize;
    size_t mdlCallbackSize;
    if (pSize != nullptr) {
        pMdlSize = &mdlSize;
        pMdlCallbackSize = &mdlCallbackSize;
    }

    bool createMdlRes = smdl_c::create(resMdl, allocator, bufferOption, viewCount, pMdlSize);
    if (!createMdlRes) {
        return false;
    }

    bool createCbRes = mCallback.create(resMdl, allocator, pMdlCallbackSize);
    if (!createCbRes) {
        remove();
        return false;
    }

    if (pSize != nullptr) {
        *pSize = mdlSize + mdlCallbackSize;
    }

    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    scnMdl->mpCallback = &mCallback;
    scnMdl->EnableScnMdlCallbackTiming(nw4r::g3d::ScnObj::CALLBACK_TIMING_ALL);

    setCallback(nullptr);
    return true;
}

void m3d::mdl_c::remove() {
    mCallback.remove();
    m3d::smdl_c::remove();
}

void m3d::mdl_c::setAnm(m3d::banm_c &anm) {
    setAnm(anm, 0);
}

void m3d::mdl_c::play() {
    m3d::bmdl_c::play();
    mCallback.calcBlend();
}

void m3d::mdl_c::setAnm(m3d::banm_c &anm, float blendFrame) {
    if (anm.getType() == m3d::banm_c::TYPE_ANM_CHR) {
        mCallback.setBlendFrame(blendFrame);
    }

    bmdl_c::setAnm(anm);
}

void m3d::mdl_c::setCallback(m3d::mdl_c::callback_c *callback) {
    mCallback.mpCallback = callback;
}
