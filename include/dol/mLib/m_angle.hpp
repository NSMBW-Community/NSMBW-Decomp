#pragma once
#include <types.h>

/// @brief 1-dimensional short angle vector.
/// @details [Used by game-specific code].
struct mAng {
    s16 mAngle; ///< The rotation.
};

/// @brief 3-dimensional short angle vector.
/// @details [Used by game-specific code].
class mAng3_c {
    public:

        /// @brief Constructs a new mAng3_c.
        mAng3_c(s16 x, s16 y, s16 z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        s16 x; ///< The rotation on the x axis.
        s16 y; ///< The rotation on the y axis.
        s16 z; ///< The rotation on the z axis.

        static mAng3_c Zero; ///< The null rotation vector.
        static mAng3_c Ex; ///< Rotation vector base for the x axis.
        static mAng3_c Ey; ///< Rotation vector base for the y axis.
        static mAng3_c Ez; ///< Rotation vector base for the z axis.
};
