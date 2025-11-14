#pragma once

#include <lib/egg/core/eggController.h>

namespace mPad {
    enum CH_e {
        MPAD_CH_0,
        MPAD_CH_1,
        MPAD_CH_2,
        MPAD_CH_3
    };

    extern EGG::CoreController *g_currentCore;
    extern EGG::CoreController *g_core[4];
};
