#include <game/mLib/m_3d.hpp>
#include <lib/nw4r/g3d/scn_mdl.hpp>

m3d::smdl_c::smdl_c() {}

m3d::smdl_c::~smdl_c() {}

bool m3d::smdl_c::create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, ulong bufferOption, int viewCount, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }

    if (bufferOption != 0) {
        mpScn = nw4r::g3d::ScnMdl::Construct(allocator, objSize, resMdl, bufferOption, viewCount);
    } else {
        mpScn = nw4r::g3d::ScnMdlSimple::Construct(allocator, objSize, resMdl, viewCount);
    }

    if (mpScn == nullptr) {
        return false;
    }

    mpScn->SetPriorityDrawOpa(127);
    mpScn->SetPriorityDrawXlu(127);

    return true;
}
