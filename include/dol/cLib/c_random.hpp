#pragma once
#include <types.h>

// NOTE: Class, function and variable names are unofficial!

class cM_rand_c {
public:
    u32 ranqd1(u32 max);
    float ranqd2();
    float ranqd3();

private:
    inline u32 ranqdStep();

    u32 seed;

    static u32 const1;
    static u32 const2;
};
