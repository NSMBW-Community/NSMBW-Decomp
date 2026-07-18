#pragma once

#include <game/bases/d_player_model_base.hpp>

class dMarioMdl_c : public dPyMdlBase_c {
public:
    dMarioMdl_c(u8 modelType);

    void fn_800cab00(int); ///< @unofficial

    u8 mPad[0x668];
};
