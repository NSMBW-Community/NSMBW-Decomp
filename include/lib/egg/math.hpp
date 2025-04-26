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

        void set(float x, float y) {
            this->x = x;
            this->y = y;
        }

        float normalise();

        /// @brief Gets the length of the vector.
        float getLength() const { return EGG::Mathf::sqrt(x * x + y * y); }
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

    /// @brief A sphere defined by a center point and a radius.
    class Sphere3f {
    public:
        Sphere3f() {}

        /// @brief Constructs a sphere from a center and radius.
        Sphere3f(const Vector3f &center, float radius) : mPos(center), mRadius(radius) {}

        Vector3f mPos;
        float mRadius;
    };
}
