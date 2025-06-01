#pragma once
#include <lib/egg/eggEffect.hpp>
#include <game/mLib/m_vec.hpp>

namespace mEf {

class effect_c : public EGG::Effect {
public:
    effect_c() {}
};

class levelEffect_c : public effect_c {
public:
    levelEffect_c() {}

};

void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);

}; // namespace mEf
