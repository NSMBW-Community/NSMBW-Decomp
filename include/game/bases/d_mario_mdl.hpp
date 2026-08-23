#pragma once

#include <game/bases/d_player_model_base.hpp>

class dMarioMdl_c : public dPyMdlBase_c {
public:
    enum HeadType_e {
        TYPE_0,
        TYPE_1
    };

    void setHeadID(HeadType_e type);
};
