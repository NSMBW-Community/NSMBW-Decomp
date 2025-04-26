#include <game/mLib/m_3d.hpp>

bool m3d::anmChrBlend_c::create(nw4r::g3d::ResMdl mdl, int count, mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }

    nw4r::g3d::AnmObjChrBlend::Construct(nullptr, objSize, mdl, count);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjChrBlend::Construct(&mAllocator, &size, mdl, count);
    return true;
}

void m3d::anmChrBlend_c::attach(int idx, nw4r::g3d::AnmObjChrRes *anmChrRes, float weight) {
    nw4r::g3d::AnmObjChrBlend *chrBlend = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrBlend>(mpObj);
    chrBlend->SetWeight(idx, weight);
    chrBlend->Attach(idx, anmChrRes);
}

void m3d::anmChrBlend_c::attach(int idx, m3d::anmChr_c *anmChr, float weight) {
    nw4r::g3d::AnmObjChrRes *res = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrRes>(anmChr->getObj());
    attach(idx, res, weight);
}

void m3d::anmChrBlend_c::detach(int idx) {
    nw4r::g3d::AnmObjChrBlend *chrBlend = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrBlend>(mpObj);
    chrBlend->Detach(idx);
}
