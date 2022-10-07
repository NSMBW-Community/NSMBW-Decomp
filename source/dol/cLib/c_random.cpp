#include <types.h>
#include <dol/cLib/c_random.hpp>

u32 cM_rand_c::mConst1 = 0x19660D;
u32 cM_rand_c::mConst2 = 0x3C6EF35F;

// This decompiled terribly but at least it matches
inline u32 cM_rand_c::ranqdStep() {

    // Variables must be declared in this order or there will be regswaps
    register u32 c, b, a, x;
    x = mSeed;
    a = mConst1;
    b = mConst2;

    // Longlong math needs to be done in asm in order to match...
    asm {
        mulhwu c, x, a
        mullw x, x, a
        addc x, x, b
        adde x, x, c
    }

    return x;
}

u32 cM_rand_c::ranqd1(u32 max) {
    mSeed = ranqdStep();
    return ((u64) mSeed * max) >> 32;
}

float cM_rand_c::ranqd2() {
    mSeed = ranqdStep();
    u32 tmp = 0x3f800000 | (mSeed >> 9 & 0x7fffff);
    return (*(float *)&tmp)-1.0f;
}

float cM_rand_c::ranqd3() {
    mSeed = ranqdStep();
    u32 tmp = 0x3f800000 | (mSeed >> 9 & 0x7fffff);
    return (*(float *)&tmp)-1.5f;
}
