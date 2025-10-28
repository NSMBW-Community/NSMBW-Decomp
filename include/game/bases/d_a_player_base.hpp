#pragma once
#include <game/bases/d_actor.hpp>

class daPlBase_c : public dActor_c {
public:
    char mPad1[0xc9e];
    float mSmallScoreOffset;
    char mPad2[0x45c];
};
