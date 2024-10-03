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
        Vector2f(f32 fx, f32 fy) { set(fx, fy); }

        /// @brief Constructs a new vector from an existing vector.
        Vector2f(const Vector2f &v) { set(v.x, v.y); }

        void set(float x, float y) {
            this->x = x;
            this->y = y;
        }

        float normalise();

        /// @brief Gets the length of the vector.
        float getLength() const { return EGG::Mathf::sqrt(x * x + y * y); }
    };
}