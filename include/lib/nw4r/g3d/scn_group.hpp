#pragma once

#include <lib/nw4r/g3d/scn_obj.hpp>

namespace nw4r {
namespace g3d {

class ScnGroup : public ScnObj {
public:
    virtual bool Insert(unsigned long, nw4r::g3d::ScnObj *);
    virtual void Remove(unsigned long);

    NW4R_G3D_TYPE_OBJ_DECL(ScnGroup);

    ScnObj **objs;
    u32 reservedCount;
    u32 count;
};

} // namespace g3d
} // namespace nw4r
