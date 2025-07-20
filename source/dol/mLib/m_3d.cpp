#include <game/mLib/m_3d.hpp>

#include <game/mLib/m_heap.hpp>
#include <game/mLib/m_video.hpp>
#include <lib/egg/gfxe/eggGfxEngine.h>
#include <lib/egg/gfxe/eggStateGX.h>
#include <lib/egg/gfxe/eggScreen.h>
#include <lib/egg/gfxe/eggDrawGX.h>
#include <lib/egg/gfxe/eggLightTexture.h>
#include <lib/nw4r/g3d/g3d.hpp>
#include <lib/nw4r/g3d/g3d_state.hpp>
#include <lib/MSL_C/string.h>

mAllocator_c *m3d::internal::l_allocator_p;
nw4r::g3d::ScnRoot *m3d::internal::l_scnRoot_p;
u32 m3d::internal::l_numLightMgr;
EGG::LightManager **m3d::internal::l_lightMgr_pp;
u32 m3d::internal::l_numFogMgr;
EGG::FogManager **m3d::internal::l_fogMgr_pp;

void m3d::initHio(const char *name) {}

bool m3d::create(EGG::Heap *heap, ulong maxChildren, ulong maxScnObj, ulong numLightObj, ulong numLightSet) {
    internal::l_allocator_p = new(heap, 4) mAllocator_c();
    if (internal::l_allocator_p == nullptr) {
        return false;
    }
    internal::l_allocator_p->attach(heap, 0x20);

    nw4r::g3d::G3dInit(true);
    nw4r::g3d::G3DState::SetRenderModeObj(mVideo::m_video->mRenderModeObj);

    size_t size;
    internal::l_scnRoot_p = nw4r::g3d::ScnRoot::Construct(
        internal::l_allocator_p, &size,
        maxChildren, maxScnObj, numLightObj, numLightSet
    );

    // Clean up on failure
    if (internal::l_scnRoot_p == nullptr) {
        delete internal::l_allocator_p;
        internal::l_allocator_p = nullptr;
        return false;
    }

    return true;
}

bool m3d::create(EGG::Heap *heap, GXPixelFmt fmt, GXColor col, ulong maxChildren, ulong maxScnObj,
                 ulong numLightObj, ulong numLightSet, ulong numLightMgr, ulong numFogMgr) {
    if (internal::l_lightMgr_pp != nullptr && internal::l_fogMgr_pp != nullptr) {
        return false;
    }

    EGG::GfxEngine::Configuration cfg;
    u16 arg1[] = { cfg.x14, cfg.x16 };
    u16 arg2[] = { cfg.x2, cfg.x2 };
    EGG::StateGX::initialize(cfg.x0, cfg.x2, col, fmt);
    EGG::Screen::Initialize(arg1, arg2, nullptr);
    EGG::Screen::SetTVModeDefault();
    EGG::DrawGX::Initialize(heap);
    EGG::LightTexture::initialize(0x20, heap);

    internal::l_lightMgr_pp = new(heap, 4) EGG::LightManager *[numLightMgr];
    if (internal::l_lightMgr_pp != nullptr) {
        internal::l_numLightMgr = numLightMgr;
        internal::l_fogMgr_pp = new(heap, 4) EGG::FogManager *[numFogMgr];
        if (internal::l_fogMgr_pp != nullptr) {
            internal::l_numFogMgr = numFogMgr;

            // Initialize the pointer array for the light and fog managers
            memset(internal::l_lightMgr_pp, 0, numLightMgr * sizeof(void *));
            memset(internal::l_fogMgr_pp, 0, numFogMgr * sizeof(void *));

            if (create(heap, maxChildren, maxScnObj, numLightObj, numLightSet)) {
                return true;
            }
        }
    }

    // Clean up on failure
    // [Oversight: both pointers can be null]
    delete[] internal::l_lightMgr_pp;
    internal::l_lightMgr_pp = nullptr;
    delete[] internal::l_fogMgr_pp;
    internal::l_fogMgr_pp = nullptr;
    return false;
}

bool m3d::createLightMgr(EGG::Heap *heap, u16 lmArg1, u16 lmArg2, u8 lmArg3, bool createFogMgr, int idx) {
    if (internal::l_lightMgr_pp == nullptr ||

        // Bounds checking
        idx < 0 || idx >= internal::l_numLightMgr || idx >= internal::l_numFogMgr ||

        // Light / fog manager must not already exist
        internal::l_lightMgr_pp[idx] != nullptr || internal::l_fogMgr_pp[idx] != nullptr) {
        return false;
    }

    EGG::Heap *prevHeap = mHeap::setCurrentHeap(heap);
    EGG::LightManager *lm = new(heap, 4) EGG::LightManager(lmArg1, lmArg2, lmArg3);
    mHeap::setCurrentHeap(prevHeap);

    if (lm != nullptr) {
        // Either we don't need a fog manager or the creation was successful
        if (!createFogMgr || m3d::createFogMgr(heap, 0x20, idx)) {
            internal::l_lightMgr_pp[idx] = lm;
            return true;
        }
    }

    // [Oversight: lm here can be null]
    delete lm;
    return false;
}

void m3d::removeLightMgr(int idx) {
    EGG::LightManager **mgrs = internal::l_lightMgr_pp;

    // [This is a bug, mgrs[idx] != nullptr should not be checked here.
    // The way it currently is, the intended functionality of removing all
    // light managers with idx < 0 is broken, since we cause UB here if idx < 0]
    if (mgrs != nullptr && mgrs[idx] != nullptr) {
        if (idx >= 0) {
            if (mgrs[idx] != nullptr) {
                delete mgrs[idx];
                mgrs[idx] = nullptr;
            }
        } else {
            // If idx is negative, delete all light managers
            for (int i = 0; i < internal::l_numLightMgr; i++) {
                if (mgrs[i] != nullptr) {
                    delete mgrs[i];
                }
            }
            delete internal::l_lightMgr_pp;
            internal::l_lightMgr_pp = nullptr;
        }
    }
    removeFogMgr(idx);
}

bool m3d::createFogMgr(EGG::Heap *heap, int fmArg, int idx) {
    if (idx < 0 || idx >= internal::l_numFogMgr || internal::l_fogMgr_pp[idx] != nullptr) {
        return false;
    }

    EGG::FogManager *fm = new(heap, 4) EGG::FogManager(fmArg);
    if (fm == nullptr) {
        return false;
    }

    internal::l_fogMgr_pp[idx] = fm;
    return true;
}

void m3d::removeFogMgr(int idx) {
    EGG::FogManager **mgrs = internal::l_fogMgr_pp;
    if (mgrs == nullptr) {
        return;
    }

    // [This is wrong, see m3d::removeLightMgr]
    if (mgrs[idx] != nullptr) {
        if (idx >= 0) {
            if (mgrs[idx] != nullptr) {
                delete mgrs[idx];
                mgrs[idx] = nullptr;
            }
        } else {
            for (int i = 0; i < internal::l_numFogMgr; i++) {
                if (mgrs[i] != nullptr) {
                    delete mgrs[i];
                }
            }
            delete internal::l_fogMgr_pp;
            internal::l_fogMgr_pp = nullptr;
        }
    }
}

nw4r::g3d::ScnRoot *m3d::getScnRoot() {
    return internal::l_scnRoot_p;
}

nw4r::g3d::Camera m3d::getCamera(int idx) {
    return internal::l_scnRoot_p->GetCamera(idx);
}

nw4r::g3d::Camera m3d::getCurrentCamera() {
    return internal::l_scnRoot_p->GetCurrentCamera();
}

int m3d::getCurrentCameraID() {
    return internal::l_scnRoot_p->currCameraID;
}

void m3d::setCurrentCamera(int id) {
    internal::l_scnRoot_p->SetCurrentCamera(id);
}

nw4r::g3d::LightSetting *m3d::getLightSettingP() {
    return &internal::l_scnRoot_p->lightSetting;
}

EGG::LightManager *m3d::getLightMgr(int idx) {
    return internal::l_lightMgr_pp[idx];
}

EGG::FogManager *m3d::getFogMgr(int idx) {
    return internal::l_fogMgr_pp[idx];
}

void m3d::drawDone(int idx) {
    if (internal::l_lightMgr_pp != nullptr && internal::l_lightMgr_pp[idx] != nullptr) {
        internal::l_lightMgr_pp[idx]->DoneDraw();
    }

    if (internal::l_fogMgr_pp != nullptr && internal::l_fogMgr_pp[idx] != nullptr) {
        internal::l_fogMgr_pp[idx]->DoneDraw();
    }
}

void m3d::drawLightMapTexture(int idx) {
    if (internal::l_lightMgr_pp != nullptr && internal::l_lightMgr_pp[idx] != nullptr) {
        internal::l_lightMgr_pp[idx]->ltMgr->drawAndCaptureTexture(0, 0, 0, 0);
    }
}

bool m3d::replaceLightMapTexture(char const *name, m3d::bmdl_c &mdl, int idx) {
    if (internal::l_lightMgr_pp == nullptr) {
        return false;
    }

    EGG::ModelEx mdlEx(mdl.getScn());
    EGG::LightTextureManager *ltm = internal::l_lightMgr_pp[idx]->ltMgr;

    return ltm->replaceModelTexture(ltm->getTextureIndex(name), &mdlEx);
}

void m3d::calcWorld(int idx) {
    if (internal::l_lightMgr_pp != nullptr && internal::l_lightMgr_pp[idx] != nullptr) {
        internal::l_lightMgr_pp[idx]->Calc(internal::l_scnRoot_p);
    }
    if (internal::l_fogMgr_pp != nullptr && internal::l_fogMgr_pp[idx] != nullptr) {
        internal::l_fogMgr_pp[idx]->Calc();
    }
    internal::l_scnRoot_p->CalcWorld();
}

void m3d::calcMaterial() {
    internal::l_scnRoot_p->CalcMaterial();
}

void m3d::calcView(int idx) {
    if (internal::l_lightMgr_pp != nullptr && internal::l_lightMgr_pp[idx] != nullptr) {
        nw4r::math::MTX34 mtx;
        nw4r::g3d::Camera cam = getCurrentCamera();
        cam.GetCameraMtx(&mtx);

        internal::l_lightMgr_pp[idx]->CalcView(mtx, getCurrentCameraID(), internal::l_scnRoot_p);
    }

    if (internal::l_fogMgr_pp != nullptr && internal::l_fogMgr_pp[idx] != nullptr) {
        internal::l_fogMgr_pp[idx]->CopyToG3D(internal::l_scnRoot_p);
    }

    internal::l_scnRoot_p->CalcView();
    internal::l_scnRoot_p->GatherDrawScnObj();
    internal::l_scnRoot_p->ZSort();
}

void m3d::drawOpa() {
    internal::l_scnRoot_p->DrawOpa();
}

void m3d::drawXlu() {
    internal::l_scnRoot_p->DrawXlu();
}

void m3d::pushBack(nw4r::g3d::ScnObj *obj) {
    internal::l_scnRoot_p->Insert(internal::l_scnRoot_p->count, obj);
}

void m3d::clear() {
    nw4r::g3d::ScnRoot *scnRoot = internal::l_scnRoot_p;
    while (scnRoot->count != 0) {
        scnRoot->Remove(scnRoot->count - 1);
    }
}

void m3d::reset() {
    nw4r::g3d::G3dReset();
    if (internal::l_lightMgr_pp != nullptr || internal::l_fogMgr_pp != nullptr) {
        EGG::StateGX::resetGXCache();
    }
}

int m3d::getNodeID(nw4r::g3d::ResMdl mdl, char const *name) {
    if (mdl.IsValid()) {
        nw4r::g3d::ResNode obj = mdl.GetResNode(name);
        if (obj.IsValid()) {
            return obj.GetID();
        }
    }
    return -1;
}

void m3d::resetMaterial() {
    GXSetNumIndStages(0);
    for (int i = 0; i < GX_MAX_TEVSTAGE; i++) {
        GXSetTevDirect(i);
    }
}

void m3d::proc_c_drawProc(nw4r::g3d::ScnProc *proc, bool drawOpa) {
    m3d::proc_c *m3dProc = (m3d::proc_c *) proc->mpHolder;
    if (drawOpa) {
        m3dProc->drawOpa();
    } else {
        m3dProc->drawXlu();
    }
}

bool m3d::proc_c::create(mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }

    mpScn = nw4r::g3d::ScnProc::Construct(allocator, objSize, proc_c_drawProc, true, true, 0);
    if (mpScn == nullptr) {
        return false;
    }

    mpScn->SetPriorityDrawOpa(127);
    mpScn->SetPriorityDrawXlu(127);

    nw4r::g3d::ScnProc *p = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnProc>(mpScn);
    p->mpHolder = this;
    return true;
}
