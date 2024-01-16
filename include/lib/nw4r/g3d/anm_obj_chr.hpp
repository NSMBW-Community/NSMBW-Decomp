#pragma once

#include <lib/nw4r/g3d/anm_obj.hpp>
#include <lib/nw4r/g3d/res_anm.hpp>
#include <lib/nw4r/math/vec.hpp>

#include <lib/rvl/mem/MEM.h>

namespace nw4r {
namespace g3d {

struct ChrAnmResult {
    u32 flags;
    nw4r::math::VEC3 pos;
    char pad[0xc];
    Mtx mtx;
};

class AnmObjChr : public AnmObj {
public:
    virtual void DetachAll();
    virtual void SetWeight(int, float);
    virtual float GetWeight(int) const;

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChr);
};

class AnmObjChrRes : public AnmObjChr {
public:    
    static AnmObjChrRes *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResAnmChr, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChr);
};

class AnmObjChrNode : public AnmObjChr {
public:
    static AnmObjChrNode *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResAnmChr, nw4r::g3d::ResMdl, bool);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChrNode);

    char pad[0x10];
    int a;
    AnmObj **arr;

    int getCount() const { return a; }
};

class AnmObjChrBlend : public AnmObjChr {
public:
    static AnmObjChrBlend *Construct(MEMAllocator*, unsigned long*, nw4r::g3d::ResMdl, int);

    NW4R_G3D_TYPE_OBJ_DECL(AnmObjChrBlend);
};

} // namespace g3d
} // namespace nw4r
