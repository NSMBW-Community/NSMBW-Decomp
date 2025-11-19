#pragma once
#include <game/mLib/m_effect.hpp>

namespace dEf {
class followEffect_c : public mEf::effect_c {
public:
    followEffect_c() {}
};

class dLevelEffect_c : public mEf::levelEffect_c {
public:
    dLevelEffect_c() {}
};

void createPlayerEffect(int, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);
void createPlayerEffect_change(int, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);
void createPlayerEffect(int, mEf::levelEffect_c *, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);
void createPlayerEffect(int, dEf::followEffect_c *, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);
void createPlayerEffect_change(int, mEf::levelEffect_c *, char const *, unsigned long, mVec3_c const *, mAng3_c const *, mVec3_c const *);

} // namespace dEf
