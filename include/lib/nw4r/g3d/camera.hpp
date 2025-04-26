#pragma once

#include <lib/nw4r/math/mtx.hpp>

namespace nw4r {
namespace g3d {
    
struct CameraData {
    char pad[0x10c];
};

class Camera {
public:
    void GetCameraMtx(nw4r::math::MTX34 *) const;

    CameraData *mpData;
};

} // namespace g3d
} // namespace nw4r
