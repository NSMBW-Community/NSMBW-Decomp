#pragma once
#include <types.h>

/// @brief [A helper class for fBase_c with unknown purpose].
/// @ingroup framework
/// @unused
/// @unofficial
class fBaHelper_c {
    s16 mStatus;
    s16 mCount;
    void **mpArr;
    void *filler1;
    s16 filler2;
    s16 mUnknown;

    virtual void vf_0x8(); ///< [Stripped out of binary]. @unused
    virtual int vf_0xc(void *); ///< [Stripped out of binary]. @unused
    virtual void vf_0x10(void *); ///< [Stripped out of binary]. @unused

public:
    void Delete(); ///< @unused @todo Document this method.
    int Load(int); ///< @unused @todo Document this method.
    bool LoadOnlyOne(); ///< @unused @todo Document this method.
};
