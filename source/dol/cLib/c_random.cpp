#include <types.h>
#include <game/cLib/c_random.hpp>
/// @file

/// @ingroup clib
namespace cRandom {
    const u32 l_RANDOM_ADD = 0x19660D; ///< Constant @p a from the @p ranqd1 algorithm.
    const u32 l_RANDOM_MUL = 0x3C6EF35F; ///< Constant @p c from the @p ranqd1 algorithm.
}

// This decompiled terribly but at least it matches
// Original algorithm: https://s3.amazonaws.com/nrbook.com/book_C210_pdf/chap07c.pdf (page 284-285)
inline u32 cM_rand_c::ranqdStep() {

    // Variables must be declared in this order or there will be regswaps
    register u32 c, b, a, x;
    x = mSeed;
    a = cRandom::l_RANDOM_ADD;
    b = cRandom::l_RANDOM_MUL;

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
