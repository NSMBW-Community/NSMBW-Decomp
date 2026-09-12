#pragma once
#include <types.h>

/// @brief Unused class with unknown purpose.
/// @ingroup bases
/// @unused
class dCurtainMng_c {
public:
    /// @brief Unused internal structure.
    /// @unused @unofficial
    struct CurtainInfo_c {
        u16 mUnk00; ///< @unused
        float mUnk04[8]; ///< @unused
    };

    void CurtainInfoAllClear(); ///< Clears all curtain information.

    CurtainInfo_c mCurtainInfo[4][4]; ///< @unused
};
