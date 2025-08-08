#pragma once

#include <game/mLib/m_3d/scn_leaf.hpp>
#include <game/mLib/m_3d/bmdl.hpp>
#include <nw4r/g3d.h>

namespace m3d {
    class proc_c : public scnLeaf_c {
    public:
        virtual scnLeaf_c::leafType_e getType() const { return scnLeaf_c::TYPE_PROC; };

        virtual void drawOpa();
        virtual void drawXlu();

        bool create(mAllocator_c *allocator, size_t *size);
    };

    void proc_c_drawProc(nw4r::g3d::ScnProc *proc, bool drawOpa);
}
