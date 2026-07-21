#pragma once

#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_effect.hpp>

namespace dEf {
    void createEffect_change(const char *, ulong, const mVec3_c *, const mAng3_c *, const mVec3_c *);
    void createPlayerEffect(int, mEf::levelEffect_c*, const char*, ulong, const mVec3_c *, const mAng3_c *, const mVec3_c *);
    void createPlayerEffect_change(int, mEf::levelEffect_c*, const char*, ulong, const mVec3_c *, const mAng3_c *, const mVec3_c *);
}
