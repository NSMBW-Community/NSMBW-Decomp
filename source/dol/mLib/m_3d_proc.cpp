#include <dol/mLib/m_3d.hpp>

void m3d::proc_c_drawProc(nw4r::g3d::ScnProc *proc, bool drawOpa) {
    m3d::proc_c *m3dProc = (m3d::proc_c *) proc->mpHolder;
    if (drawOpa) {
        m3dProc->drawOpa();
    } else {
        m3dProc->drawXlu();
    }
}

bool m3d::proc_c::create(mAllocator_c *allocator, unsigned long *smth) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }
    unsigned long idk;
    if (smth == nullptr) {
        smth = &idk;
    }
    mpScn = nw4r::g3d::ScnProc::Construct(allocator, smth, proc_c_drawProc, true, true, 0);
    if (mpScn == nullptr) {
        return false;
    }
    mpScn->SetPriorityDrawOpa(0x7f);
    mpScn->SetPriorityDrawXlu(0x7f);
    nw4r::g3d::ScnProc *p = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnProc>(mpScn);
    p->mpHolder = this;
    return true;
}
