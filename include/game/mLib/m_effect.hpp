#pragma once
#include <lib/egg/eggEffect.hpp>
#include <game/mLib/m_vec.hpp>

namespace mEf {

class effect_c : public EGG::Effect {
public:
    effect_c() {}

    u8 mPad[0x94];
};

class levelEffect_c : public effect_c {
public:
    levelEffect_c() : m_114(0), m_118(0), m_11c(0), m_11d(0), m_120(0), m_124(0) {}
    ~levelEffect_c() { cleanup(); }

    void cleanup();

    u32 m_114, m_118;
    u8 m_11c, m_11d;
    u32 m_120, m_124;
};

void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);

}; // namespace mEf
