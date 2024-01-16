#pragma once

#include <lib/nw4r/g3d/scn_leaf.hpp>

namespace nw4r {
namespace g3d {

class ScnProc : public ScnLeaf {
public:
    virtual ~ScnProc();

    static ScnProc *Construct( MEMAllocator*, unsigned long*, void (*)( nw4r::g3d::ScnProc*, bool ), bool, bool, unsigned long );

    NW4R_G3D_TYPE_OBJ_DECL(ScnProc);

    typedef void ScnObjCb(ScnProc *, bool);

    u32 unk1;
    ScnObjCb *cb;
    void *mpHolder;
};

} // namespace g3d
} // namespace nw4r
