#pragma once

#include <math.h>

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

    typedef Math<float> Mathf;
}
