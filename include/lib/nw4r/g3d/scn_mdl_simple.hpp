#pragma once

#include <lib/nw4r/g3d/anm_obj.hpp>
#include <lib/nw4r/g3d/res_mdl.hpp>
#include <lib/nw4r/g3d/scn_leaf.hpp>

namespace nw4r {
namespace g3d {

class ScnMdlSimple : public ScnLeaf {
public:
    enum AnmObjType {
        ZERO = 0,
        SIX = 6
    };

    virtual void SetAnmObj(nw4r::g3d::AnmObj *, nw4r::g3d::ScnMdlSimple::AnmObjType);
    virtual void RemoveAnmObj(nw4r::g3d::AnmObj *);
    virtual void RemoveAnmObj(nw4r::g3d::ScnMdlSimple::AnmObjType);
    virtual nw4r::g3d::AnmObj *GetAnmObj(nw4r::g3d::ScnMdlSimple::AnmObjType) const;
    virtual nw4r::g3d::AnmObj *GetAnmObj(nw4r::g3d::ScnMdlSimple::AnmObjType);

    static ScnMdlSimple *Construct(MEMAllocator *, unsigned long *, ResMdl, int);

    void EnableScnMdlCallbackTiming(ScnObj::Timing);

    bool GetScnMtxPos(nw4r::math::MTX34 *, nw4r::g3d::ScnObj::ScnObjMtxType, unsigned long) const;

    ResMdl mpResMdl;
    char pad2[0x30]; // not sure how much of the pad is in here vs in ObjMdl
    void *mpCallback;

    NW4R_G3D_TYPE_OBJ_DECL(ScnMdlSimple);
};

} // namespace g3d
} // namespace nw4r
