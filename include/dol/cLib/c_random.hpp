#pragma once
#include <types.h>

// NOTE: Class, function and variable names are unofficial!

class cM_rand_c {
public:
    cM_rand_c(u32 seed) { mSeed = seed; };
    u32 ranqd1(u32 max);
    float ranqd2();
    float ranqd3();

    u32 mSeed;

private:
    inline u32 ranqdStep();

    static u32 mConst1;
    static u32 mConst2;
};
