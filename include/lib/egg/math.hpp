#pragma once

#include <lib/nw4r/math/vec.hpp>
#include <lib/MSL_C/math.h>

namespace EGG {
    /// @note Unfinished
    template <class T>
    class Math {
    public:
        static T sqrt(T v);

        static T abs(T v) {
            return v < 0 ? -v : v;
        }
    };

    template <>
    float Math<float>::abs(float v) {
        return fabs(v);
    }

    typedef Math<float> Mathf;

    /// @brief A two-dimensional floating point vector.
    class Vector2f : public nw4r::math::VEC2 {
    public:
        /// @brief Constructs an empty vector.
        Vector2f() {}

        ~Vector2f() {}

        /// @brief Constructs a vector from two floating point values.
        Vector2f(const f32 &fx, const f32 &fy) { set(fx, fy); }

        // Vector2f(const Vector2f &v) : nw4r::math::VEC2(v) { }
        Vector2f(const Vector2f &v) { set(v.x, v.y); }

        void set(float x, float y) {
            this->x = x;
            this->y = y;
        }

        float normalise();
    };
}
