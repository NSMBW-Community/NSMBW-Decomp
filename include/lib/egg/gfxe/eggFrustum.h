#pragma once

#include <nw4r/g3d.h>

namespace EGG {

class Frustum {
public:
    enum Flag {
        FLAG_DIRTY = BIT_FLAG(0),
        FLAG_0x02 = BIT_FLAG(1),
        FLAG_0x04 = BIT_FLAG(2),
        FLAG_0x08 = BIT_FLAG(3),
        FLAG_0x20 = BIT_FLAG(5),
        FLAG_0x40 = BIT_FLAG(6),
    };

    enum ProjectionType {
        PROJ_ORTHO,
        PROJ_PERSP
    };

    enum CanvasMode {
        CANVASMODE_0,
        CANVASMODE_1,
    };

    ProjectionType mProjType;
    CanvasMode mCanvasMode;
    nw4r::math::VEC2 mSize;
    f32 mFovY;
    f32 mTanFovY;
    f32 mNearZ;
    f32 mFarZ;
    nw4r::math::VEC2 mOffset;
    nw4r::math::VEC3 mScale;
    mutable u16 mFlags;

    Frustum(ProjectionType projType, const nw4r::math::VEC2 &scale, f32 nearZ, f32 farZ, CanvasMode canvasMode);
    Frustum(const Frustum &frustum);

    virtual ~Frustum() {}
    virtual void SetProjectionGX() const;
    virtual void CopyToG3D(nw4r::g3d::Camera camera) const;
    void ResetOrthographic(f32 top, f32 bottom, f32 left, f32 right, f32 nearZ, f32 farZ);
};

} // namespace EGG
