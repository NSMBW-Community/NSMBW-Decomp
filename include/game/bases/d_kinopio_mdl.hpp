#pragma once

#include <game/bases/d_player_model.hpp>

class dKinopioMdl_c : public dPlayerMdl_c {
    u8 mPad[0x58];
public:
    dKinopioMdl_c(u8 modelType);
};
