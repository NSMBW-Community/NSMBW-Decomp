#pragma once

#include <lib/egg/core/eggDisplay.h>

namespace EGG {

class AsyncDisplay : public Display {
public:
    AsyncDisplay(u8 param);

    void FUN_802bb750(); ///< @unofficial
    void clearEFB();

    u8 mPad[0xa8];
};

} // namespace EGG
