#pragma once
#include <game/mLib/m_3d/bmdl.hpp>

namespace m3d {
    class smdl_c : public bmdl_c {
    public:
        smdl_c();
        virtual ~smdl_c();

        bool create(nw4r::g3d::ResMdl resMdl, mAllocator_c *allocator, ulong bufferOption, int viewCount, size_t *objSize);
    };
}
