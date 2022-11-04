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

        s16 x; ///< The rotation on the X axis.
        s16 y; ///< The rotation on the Y axis.
        s16 z; ///< The rotation on the Z axis.

        static mAng3_c Zero; ///< The null rotation vector.
        static mAng3_c Ex; ///< Unit vector for the rotation on the X axis.
        static mAng3_c Ey; ///< Unit vector for the rotation on the Y axis.
        static mAng3_c Ez; ///< Unit vector for the rotation on the Z axis.
};
