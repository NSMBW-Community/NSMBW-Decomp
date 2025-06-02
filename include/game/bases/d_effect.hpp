#pragma once
#include <game/mLib/m_effect.hpp>

namespace dEf {

void createPlayerEffect(int, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);
void createPlayerEffect(int, mEf::levelEffect_c *, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);

class followEffect_c : public mEf::effect_c {
public:
    followEffect_c() {}

    u8 mPad[0x8];
};

} // namespace dEf
