#pragma once
#include <types.h>

/// @unused [A helper class for fBase_c with unknown purpose].
/// @unofficial
class fBaHelper_c {
    s16 mStatus;
    s16 mCount;
    void **mpArr;
    void *filler1;
    s16 filler2;
    s16 mUnknown;

    virtual void vf_0x8(); ///< @unused [Stripped out of binary].
    virtual int vf_0xc(void *); ///< @unused [Stripped out of binary].
    virtual void vf_0x10(void *); ///< @unused [Stripped out of binary].

public:
    void Delete(); ///< @unused @todo Document this method.
    int Load(int); ///< @unused @todo Document this method.
    bool LoadOnlyOne(); ///< @unused @todo Document this method.
};
