#pragma once

#include <lib/nw4r/math.h>

class Snd2DCalc {
public:
    struct Snd2DAmbParam {
        Snd2DAmbParam(float v) : m_00(v) {}
        float m_00;
    };

    void calcAmbParam(Snd2DAmbParam *, const nw4r::math::VEC2 &, unsigned long);
};
