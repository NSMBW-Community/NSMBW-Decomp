#pragma once

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

        static T zero() { return 0; }
    };

    template <>
    float Math<float>::abs(float v) {
        return fabs(v);
    }

    typedef Math<float> Mathf;
}
