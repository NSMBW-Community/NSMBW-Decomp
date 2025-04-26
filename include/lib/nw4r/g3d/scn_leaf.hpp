#pragma once

#include <lib/nw4r/g3d/scn_obj.hpp>

namespace nw4r {
namespace g3d {

class ScnLeaf : public ScnObj {
public:
    nw4r::math::VEC3 mScale;

    void SetScale(float x, float y, float z) { mScale.x = x; mScale.y = y; mScale.z = z; }
    void SetScale(const nw4r::math::VEC3 &scale) { mScale = scale; }

    NW4R_G3D_TYPE_OBJ_DECL(ScnLeaf);
};

} // namespace g3d
} // namespace nw4r
