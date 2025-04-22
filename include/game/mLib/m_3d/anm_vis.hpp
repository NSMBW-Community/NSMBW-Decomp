#pragma once
#include <game/mLib/m_3d/bmdl.hpp>
#include <game/mLib/m_3d/fanm.hpp>

namespace m3d {
    class anmVis_c : public fanm_c {
    public:
        virtual banm_c::anmType_e getType() const { return banm_c::TYPE_ANM_VIS; };

        bool create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmVis anmVis, mAllocator_c *allocator, size_t *objSize);
        void setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmVis anmVis, m3d::playMode_e playMode);
        void setFrmCtrlDefault(nw4r::g3d::ResAnmVis &anmVis, m3d::playMode_e playMode);
    };
}
