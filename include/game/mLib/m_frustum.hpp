#pragma once

#include <types.h>
#include <game/mLib/m_aabb.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/mLib/m_sphere.hpp>

/// @brief A view frustum, used to cull objects outside of the camera's field of view.
/// @ingroup mlib
class mFrustum_c : nw4r::math::FRUSTUM {
public:
    mFrustum_c() {} ///< Constructs an empty frustum.

    /**
    * @brief Builds a symmetric perspective frustum.
    * @param fovy The vertical field of view, in degrees.
    * @param aspect The aspect ratio (width / height).
    * @param near The distance to the near clipping plane.
    * @param far The distance to the far clipping plane.
    * @param cameraMtx The matrix defining the camera's transform in world space.
    */
    void set(f32 fovy, f32 aspect, f32 n, f32 f, const mMtx_c &cameraMtx);

    /**
    * @brief Builds an arbitrary perspective or orthographic frustum.
    * @param top The top boundary.
    * @param bottom The bottom boundary.
    * @param left The left boundary.
    * @param right The right boundary.
    * @param near The distance to the near clipping plane.
    * @param far The distance to the far clipping plane.
    * @param cameraMtx The matrix defining the camera's transform in world space.
    * @param orthographic @p true for orthographic projection, @p false for perspective projection.
    */
    void set(f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far, const mMtx_c &cameraMtx, bool orthographic);

    /// @brief Checks whether the given sphere is at least partially inside the frustum.
    bool intersectSphere(const mSphere_c *sphere) const;

    /// @brief Checks whether the given bounding box is at least partially inside the frustum.
    bool intersectAABB(const mAABB_c *box) const;
};
