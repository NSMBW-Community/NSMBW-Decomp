#pragma once

#include <types.h>
#include <game/mLib/m_aabb.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/mLib/m_sphere.hpp>

/// @brief A view frustum, used to cull objects outside of the camera's field of view.
/// @ingroup mlib
class mFrustum_c {
public:
    /// @brief Constructs an empty frustum.
    mFrustum_c() {}

    /// @brief Builds a symmetric perspective frustum.
    void set(f32 fovy, f32 aspect, f32 n, f32 f, const mMtx_c &mtx);

    /// @brief Builds an arbitrary perspective or orthographic frustum.
    void set(f32 t, f32 b, f32 l, f32 r, f32 n, f32 f, const mMtx_c &mtx, bool orthographic);

    /// @brief Checks whether the given sphere is at least partially inside the frustum.
    bool intersectSphere(const mSphere_c *pSphere) const;

    /// @brief Checks whether the given bounding box is at least partially inside the frustum.
    bool intersectAABB(const mAABB_c *pBox) const;

private:
    /// @brief The frustum corner points.
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

    /// @brief The frustum planes.
    enum Plane {
        PLANE_L,
        PLANE_R,
        PLANE_N,
        PLANE_F,
        PLANE_T,
        PLANE_B,

        PLANE_MAX
    };

    nw4r::math::MTX34 mCamMtx; ///< The camera matrix.
    nw4r::math::PLANE mPlaneL; ///< The left plane, in camera space.
    nw4r::math::PLANE mPlaneR; ///< The right plane, in camera space.
    nw4r::math::PLANE mPlaneT; ///< The top plane, in camera space.
    nw4r::math::PLANE mPlaneB; ///< The bottom plane, in camera space.
    f32 mNearZ; ///< The near plane depth, in camera space.
    f32 mFarZ; ///< The far plane depth, in camera space.
    mAABB_c mBox; ///< The bounding box enclosing the frustum.
    nw4r::math::PLANE mPlanes[PLANE_MAX]; ///< The frustum planes, in world space.
};
