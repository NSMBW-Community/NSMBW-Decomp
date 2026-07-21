#pragma once
#include <types.h>

/// @brief A helper class for fBase_c for loading archive files.
/// @ingroup framework
/// @unused
class fArcLoad_c {
    ///< @unofficial
    enum Status_e {
        STATUS_UNLOADED,
        STATUS_LOADING,
        STATUS_LOADED
    };

    s16 mStatus;
    s16 mCount;
    const char **mpArcList;
    u8 mPad[6];
    s16 mUnknown;

    virtual void vf_0x8(); ///< [Stripped out of binary]. @unofficial @unused
    virtual int loadArc(const char *); ///< [Stripped out of binary]. @unofficial @unused
    virtual void deleteArc(const char *); ///< [Stripped out of binary]. @unofficial @unused

public:
    void arcListDelete(); ///< @unused @todo Document this method.
    int arcListLoad(int loadMany); ///< @unused @todo Document this method.
    bool checkDelete(); ///< @unused @todo Document this method.
};
