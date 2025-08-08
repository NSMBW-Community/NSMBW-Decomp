#pragma once

#include <lib/egg/math/eggMath.h>
#include <nw4r/math.h>
#include <types.h>

namespace EGG {
    /// @brief A two-dimensional floating point vector.
    class Vector2f : public nw4r::math::VEC2 {
    public:
        /// @brief Constructs an empty vector.
        Vector2f() {}

        ~Vector2f() {}

        /// @brief Constructs a vector from two floating point values.
        Vector2f(f32 fx, f32 fy) { set(fx, fy); }

        void set(float x, float y) {
            this->x = x;
            this->y = y;
        }

        float normalise();

        /// @brief Gets the squared length of the vector.
        float squaredLength() const { return x * x + y * y; }

        /// @brief Gets the length of the vector.
        float length() const { return EGG::Mathf::sqrt(squaredLength()); }
    };

    /// @brief A three-dimensional floating point vector.
    class Vector3f : public nw4r::math::VEC3 {
    public:
        /// @brief Constructs an empty vector.
        Vector3f() {}

        ~Vector3f() {}

        /// @brief Constructs a vector from two floating point values.
        Vector3f(f32 fx, f32 fy, f32 fz) { set(fx, fy, fz); }

        void set(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };

}
