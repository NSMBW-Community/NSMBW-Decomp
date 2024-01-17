#include <dol/mLib/m_3d.hpp>

m3d::mdl_c::mdlCallback_c::mdlCallback_c() {
    countObjs = 0;
    objs = nullptr;
    mpCallback = nullptr;
    mpAllocator = nullptr;
}

m3d::mdl_c::mdlCallback_c::~mdlCallback_c() {}

void m3d::mdl_c::mdlCallback_c::ExecCallbackA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw) {
    unsigned short num = cw->unk6;

     nw4r::g3d::ChrAnmResult *sel = &objs[num];

    if (mCalcRatio.b1 && !mCalcRatio.isEnd()) {
        if (!mCalcRatio.b2) {
            *anmRes = *sel;
        } else {
            float slerpParam = mCalcRatio.getSlerpParam();
            float scaleSel = mCalcRatio.getScaleSel();
            float scaleAnm = mCalcRatio.getScaleAnm();

            u32 flags = anmRes->flags;
            
            if ((flags & 8) == 0) {
                anmRes->scale.x = anmRes->scale.x * scaleAnm + sel->scale.x * scaleSel;
                anmRes->scale.y = anmRes->scale.y * scaleAnm + sel->scale.y * scaleSel;
                anmRes->scale.z = anmRes->scale.z * scaleAnm + sel->scale.z * scaleSel;
            } else {
                anmRes->scale.x = scaleAnm + sel->scale.x * scaleSel;
                anmRes->scale.y = scaleAnm + sel->scale.y * scaleSel;
                anmRes->scale.z = scaleAnm + sel->scale.z * scaleSel;
            }

            QUAT mtx, mtx2;
            C_QUATMtx(&mtx, &sel->mtx);
            if ((flags & 0x20) == 0) {
                C_QUATMtx(&mtx2, &anmRes->mtx);
            } else {
                mtx2.x = 0;
                mtx2.y = 0;
                mtx2.z = 0;
                mtx2.w = 1;
            }

            C_QUATSlerp(&mtx, &mtx2, &mtx, slerpParam);

            nw4r::math::VEC3 cpy;
            cpy.x = anmRes->mtx[0][3];
            cpy.y = anmRes->mtx[1][3];
            cpy.z = anmRes->mtx[2][3];
            PSMTXQuat(&anmRes->mtx, &mtx);
            anmRes->mtx[0][3] = cpy.x;
            anmRes->mtx[1][3] = cpy.y;
            anmRes->mtx[2][3] = cpy.z;

            if ((flags & 0x40) == 0) {
                anmRes->mtx[0][3] = cpy.x * scaleAnm;
                anmRes->mtx[1][3] = cpy.y * scaleAnm;
                anmRes->mtx[2][3] = cpy.z * scaleAnm;
            }

            anmRes->mtx[0][3] += sel->mtx[0][3] * scaleSel;
            anmRes->mtx[1][3] += sel->mtx[1][3] * scaleSel;
            anmRes->mtx[2][3] += sel->mtx[2][3] * scaleSel;

            anmRes->flags &= ~((1 << 31) | (1 << 6) | (1 << 5) | (1 << 3));
            
            *sel = *anmRes;
        }
    } else {
        *sel = *anmRes;
    }
    
    if (mpCallback != nullptr) {
        mpCallback->timingA(num, anmRes, resMdl);
    }
}

void m3d::mdl_c::mdlCallback_c::ExecCallbackB(nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw) {
    unsigned short num = cw->unk6;
    if (mpCallback != nullptr) {
        mpCallback->timingB(num, manip, resMdl);
    }
    unsigned int ne = resMdl.GetResNodeNumEntries();
    unsigned int tmp = ((num + 1) / ne);
    cw->unk6 = (num + 1) - tmp * ne;
}

void m3d::mdl_c::mdlCallback_c::ExecCallbackC(nw4r::math::MTX34 *mtx, nw4r::g3d::ResMdl resMdl, nw4r::g3d::FuncObjCalcWorld *cw) {
    if (mpCallback != nullptr) {
        mpCallback->timingC(mtx, resMdl);
    }
    mCalcRatio.offUpdate();
}

bool m3d::mdl_c::mdlCallback_c::create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, unsigned long *p3) {
    
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    unsigned long buf1 = 0;
    if (p3 == nullptr) {
        p3 = &buf1;
    }

    countObjs = resMdl.GetResNodeNumEntries();

    size_t objsSize = countObjs * sizeof(nw4r::g3d::ChrAnmResult);

    objs = (nw4r::g3d::ChrAnmResult *) MEMAllocFromAllocator(allocator, objsSize);

    if (objs == nullptr) {
        return false;
    }

    *p3 = ((objsSize + ((*p3 + 3) & 0xFFFFFFFC) + 3) & 0xFFFFFFFC);

    nw4r::g3d::ChrAnmResult *curr = objs;
    for (int i = 0; i < countObjs; i++) {
        curr->scale.set(1, 1, 1);
        PSMTXIdentity(&curr->mtx);
        curr++;
    }

    mpAllocator = allocator;
    
    return true;
}

void m3d::mdl_c::mdlCallback_c::remove() {
    mCalcRatio.remove();
    if (objs != nullptr) {
        mpAllocator->free(objs);
    }
    objs = nullptr;
    mpAllocator = nullptr;
}

void m3d::mdl_c::mdlCallback_c::setBlendFrame(float frame) {
    mCalcRatio.set(frame);
}

void m3d::mdl_c::mdlCallback_c::calcBlend() {
    if (!mCalcRatio.isEnd()) {
        mCalcRatio.calc();
    }
}

m3d::mdl_c::mdl_c() {}
m3d::mdl_c::~mdl_c() {}

bool m3d::mdl_c::create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, unsigned long p3, int p4, unsigned long *p5) {
    unsigned long buf1;
    unsigned long buf2;
    
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    unsigned long *arg1 = nullptr;
    unsigned long *arg2 = nullptr;
    if (p5 != nullptr) {
        arg1 = &buf1;
        arg2 = &buf2;
    }
    bool res = smdl_c::create(resMdl, allocator, p3, p4, arg1);
    if (!res) {
        return false;
    }

    bool cbCreateRes = mCallback.create(resMdl, allocator, arg2);

    if (!cbCreateRes) {
        remove();
        return false;
    }

    if (p5 != nullptr) {
        *p5 = buf1 + buf2;
    }
    
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    scnMdl->mpCallback = &mCallback;
    scnMdl->EnableScnMdlCallbackTiming(nw4r::g3d::ScnObj::TIMING_7);
    
    setCallback(0);
    
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
    if (anm.getType() == 0) {
        mCallback.setBlendFrame(blendFrame);
    }
    bmdl_c::setAnm(anm);
}

void m3d::mdl_c::setCallback(m3d::mdl_c::callback_c *callback) {
    mCallback.mpCallback = callback;
}
