#include <dol/mLib/m_3d.hpp>

#include <lib/nw4r/g3d/scn_mdl.hpp>

m3d::smdl_c::smdl_c() {}

m3d::smdl_c::~smdl_c() {}

bool m3d::smdl_c::create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, unsigned long p3, int p4, unsigned long *p5) {
    unsigned long buf;
    
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }
    if (p5 == nullptr) {
        p5 = &buf;
    }
    if (p3 != 0) {
        mpScn = nw4r::g3d::ScnMdl::Construct(allocator, p5, resMdl, p3, p4);
    } else {
        mpScn = nw4r::g3d::ScnMdlSimple::Construct(allocator, p5, resMdl, p4);
    }

    if (mpScn == nullptr) {
        return false;
    }

    mpScn->SetPriorityDrawOpa(127);
    mpScn->SetPriorityDrawXlu(127);

    return true;
}
