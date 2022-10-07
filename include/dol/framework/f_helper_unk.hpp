#pragma once
#include <types.h>

/// @brief [A helper class for fBase_c with unknown purpose].
/// @note Unofficial name.
class fBaHelper_c {
    s16 mStatus;
    s16 mCount;
    void **mpArr;
    void *filler1;
    s16 filler2;
    s16 mUnknown;

public:
    virtual void vf_0x8(); ///< [Stripped out of binary].
    virtual int vf_0xc(void *); ///< [Stripped out of binary].
    virtual void vf_0x10(void *); ///< [Stripped out of binary].

    void Delete(); ///< [TODO: document this method]
    int Load(int); ///< [TODO: document this method]
    bool LoadOnlyOne(); ///< [TODO: document this method]
};
