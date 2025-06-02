#pragma once
#include <game/mLib/m_effect.hpp>

namespace dEf {

class followEffect_c : public mEf::effect_c {
public:
    followEffect_c() {}

    u8 mPad[0x8];
};

} // namespace dEf
