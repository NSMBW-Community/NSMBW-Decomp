#ifndef NW4R_MATH_GEOMETRY_H
#define NW4R_MATH_GEOMETRY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace math {

// Forward declarations
struct AABB;

enum IntersectionResult {
    INTERSECTION_NONE,
    INTERSECTION_1,
    INTERSECTION_2,

    INTERSECTION_LINE3_ON_PLANE = INTERSECTION_2,
    INTERSECTION_RAY3_ON_PLANE = INTERSECTION_2,
    INTERSECTION_SEGMENT3_ON_PLANE = INTERSECTION_2,

    INTERSECTION_OUTSIDE = 0,
    INTERSECTION_INSIDE,
    INTERSECTION_INTERSECT
};

bool IntersectionAABB(const AABB* pA, const AABB* pB);

/******************************************************************************
 *
 * Plane
 *
 ******************************************************************************/
struct PLANE {
    PLANE() {}

    f32 Test(const VEC3& rPoint) const {
        return d + VEC3Dot(&n, &rPoint);
    }

    void Set(const VEC3* p0, const VEC3* p1, const VEC3* p2);

    VEC3 n; // at 0x0
    f32 d;  // at 0xC
};

/******************************************************************************
 *
 * Axis-aligned bounding box
 *
 ******************************************************************************/
struct AABB {
    AABB() {}

    void Set(const VEC3* pPoints, unsigned int num);
    void Set(const AABB* pBox, const MTX34* pMtx);

    VEC3 min; // at 0x0
    VEC3 max; // at 0xC
};

/******************************************************************************
 *
 * Frustum
 *
 ******************************************************************************/
class FRUSTUM {
private:
    enum Point {
        POINT_NEAR_TL,
        POINT_NEAR_TR,
        POINT_NEAR_BR,
        POINT_NEAR_BL,

        POINT_FAR_TL,
        POINT_FAR_TR,
        POINT_FAR_BR,
        POINT_FAR_BL,

        POINT_MAX
    };

    enum Plane {
        PLANE_L,
        PLANE_R,
        PLANE_N,
        PLANE_F,
        PLANE_T,
        PLANE_B,

        PLANE_MAX
    };

public:
    void Set(f32 fovy, f32 aspect, f32 n, f32 f, const MTX34& rCamMtx);
    void Set(f32 t, f32 b, f32 l, f32 r, f32 n, f32 f, const MTX34& rCamMtx);

    IntersectionResult IntersectAABB_Ex(const AABB* pBox) const;

private:
    MTX34 mCamMtx;            // at 0x0
    PLANE mPlaneL;            // at 0x30
    PLANE mPlaneR;            // at 0x40
    PLANE mPlaneT;            // at 0x50
    PLANE mPlaneB;            // at 0x60
    f32 mNearZ;               // at 0x70
    f32 mFarZ;                // at 0x74
    AABB mBox;                // at 0x78
    PLANE mPlanes[PLANE_MAX]; // at 0x90
};

} // namespace math
} // namespace nw4r

#endif
