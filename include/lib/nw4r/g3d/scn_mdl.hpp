#pragma once

#include <lib/nw4r/g3d/scn_mdl_simple.hpp>

namespace nw4r {
namespace g3d {

class ScnMdl : public ScnMdlSimple {
public:
    virtual ~ScnMdl();

    static ScnMdl *Construct(MEMAllocator *, unsigned long *, ResMdl, unsigned long, int);

    class CopiedMatAccess {
    public:
        char fill[0x34];
        CopiedMatAccess(ScnMdl *, unsigned long);

        ResMatMisc GetResMatMisc(bool);
        ResMatTevColor GetResMatTevColor(bool);
    };

    NW4R_G3D_TYPE_OBJ_DECL(ScnMdl);
};

} // namespace g3d
} // namespace nw4r
