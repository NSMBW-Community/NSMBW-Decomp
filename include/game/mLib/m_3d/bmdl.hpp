#pragma once
#include <game/mLib/m_3d/banm.hpp>
#include <game/mLib/m_3d/scn_leaf.hpp>
#include <nw4r/g3d.h>

namespace m3d {
    class bmdl_c : public scnLeaf_c {
    public:
        bmdl_c() : mpAnm(nullptr) {}
        virtual ~bmdl_c();

        virtual scnLeaf_c::leafType_e getType() const { return TYPE_BMDL; }
        virtual void remove();
        virtual void setAnm(m3d::banm_c &anm);
        virtual void play();

        bool getNodeWorldMtx(ulong idx, nw4r::math::MTX34 *mtx) const;
        bool getNodeWorldMtxMultVecZero(ulong idx, nw4r::math::VEC3 &vec) const;

        nw4r::g3d::ResMdl getResMdl() const;
        nw4r::g3d::ResMat getResMat(size_t idx) const;
        void removeAnm(nw4r::g3d::ScnMdlSimple::AnmObjType objType);

        void setTevColor(ulong idx, _GXTevRegID regID, _GXColor color, bool markDirty);
        void setTevColorAll(_GXTevRegID regID, _GXColor color, bool markDirty);
        void setTevKColor(ulong idx, _GXTevKColorID colID, _GXColor color, bool markDirty);
        void setTevKColorAll(_GXTevKColorID colID, _GXColor color, bool markDirty);

    protected:
        banm_c *mpAnm;
    };
}
