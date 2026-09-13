#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

/// @brief Provides common utilities.
/// @ingroup clib
namespace cLib {
    void memSet(void *dst, int val, ulong size); ///< See memset.

    /// @brief Checks if a floating-point value is effectively zero within epsilon tolerance.
    /// @param val The float value to evaluate.
    /// @return @p true if the absolute value is less than FLT_EPSILON, @p false otherwise.
    inline bool isZero(float val) {
        return (std::fabs(val) < FLT_EPSILON);
    }

    /// @brief Calculates the yaw (Y-axis) orientation angle from an origin position to a target position.
    /// @param vec1 The origin position vector.
    /// @param vec2 The target position vector.
    /// @return The calculated yaw value.
    s16 targetAngleY(const mVec3_c &vec1, const mVec3_c &vec2);

    /// @brief Calculates the pitch (X-axis) orientation angle from an origin position to a target position.
    /// @param vec1 The origin position vector.
    /// @param vec2 The target position vector.
    /// @return The calculated pitch value.
    s16 targetAngleX(const mVec3_c &vec1, const mVec3_c &vec2);

    /**
     * @brief Smoothly moves @p currentPos towards @p targetPos using proportional scaling.
     * @details If the applied step overshoots the target, the position is snapped exactly to the target.
     * @param currentPos The position to be updated.
     * @param targetPos The target position.
     * @param ratio The smoothing ratio (between @p 0.0f and @p 1.0f). Higher value means snappier movement.
     * @param maxStep The maximum step for a single update.
     * @param minStep The minimum step to prevent jittering.
     * @return The absolute remaining distance to the target position.
     */
    float addCalcPos(mVec3_c *currentPos, const mVec3_c &targetPos, float ratio, float maxStep, float minStep);

    /** @brief Moves @p currentPos towards @p targetPos by a fixed @p step amount.
     * @details If the applied step overshoots the target, the position is snapped exactly to the target.
     * @param currentPos The position to be updated.
     * @param targetPos The target position.
     * @param step The step value.
     * @return @p true if the position reached the target, @p false otherwise.
     */
    bool chasePos(mVec3_c *currentPos, const mVec3_c &targetPos, float step);

} // namespace cLib
