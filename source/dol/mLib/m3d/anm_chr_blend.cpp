#include <dol/mLib/m_3d.hpp>

bool m3d::anmChrBlend_c::create(nw4r::g3d::ResMdl mdl, int v, mAllocator_c *allocator, unsigned long *idk) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }
    unsigned long tmp;
    if (idk == nullptr) {
        idk = &tmp;
    }
    nw4r::g3d::AnmObjChrBlend::Construct(nullptr, idk, mdl, v);
    if (!createAllocator(allocator, idk)) {
        return false;
    }
    mpObj = nw4r::g3d::AnmObjChrBlend::Construct(&mAllocator, &tmp, mdl, v);
    return true;
}

void m3d::anmChrBlend_c::attach(int num, nw4r::g3d::AnmObjChrRes *chrRes, float weight) {
    nw4r::g3d::AnmObjChrBlend *chrBlend = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrBlend>(mpObj);
    chrBlend->SetWeight(num, weight);
    chrBlend->Attach(num, chrRes);
}

void m3d::anmChrBlend_c::attach(int num, m3d::anmChr_c *chrRes, float weight) {
    nw4r::g3d::AnmObjChrRes *res = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrRes>(chrRes->mpObj);
    attach(num, res, weight);
}

void m3d::anmChrBlend_c::detach(int num) {
    nw4r::g3d::AnmObjChrBlend *chrBlend = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrBlend>(mpObj);
    chrBlend->Detach(num);
}
