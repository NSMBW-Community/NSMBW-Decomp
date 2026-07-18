#pragma once

#include <game/bases/d_player_model_base.hpp>

class dMarioMdl_c : public dPyMdlBase_c {
public:
    enum HeadType_e {
        TYPE_0,
        TYPE_1
    };

    dMarioMdl_c(u8 modelType);

    void setHeadID(HeadType_e type); ///< @unofficial

    u8 mPad[0x668];
};
