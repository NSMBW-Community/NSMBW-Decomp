#pragma once
#include <types.h>

namespace cM {

    // Conversion utilities
    s16 rad2s(float rad);
    s16 atan2s(float sin, float cos);

    // RNG utilities
    void initRnd(ulong seed);
    float rnd();
    int rndInt(int max);
    float rndF(float max);

} // namespace cM
