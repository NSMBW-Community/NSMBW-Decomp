#pragma once

#include <lib/egg/math/eggVector.h>

namespace EGG {
    /// @brief A sphere defined by a center point and a radius.
    class Sphere3f {
    public:
        Sphere3f() {}

        /// @brief Constructs a sphere from a center and radius.
        Sphere3f(const Vector3f &center, float radius) : mCenter(center), mRadius(radius) {}

        Vector3f mCenter;
        float mRadius;
    };
}
