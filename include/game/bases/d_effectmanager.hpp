#pragma once

#include <game/mLib/m_vec.hpp>
#include <lib/egg/core/eggHeap.h>

class EffectManager_c {
public:
    static void create(EGG::Heap *heap1, EGG::Heap *heap2);
    static void setResource(void *breff, void *breft);
    static void SetIceBallMissshitEffect(mVec3_c *);
    static void SetIceBallEffect(mVec3_c *);
    static void calcGroupForWm();
    static void calcGroup();
    static void SetVsHitEffect(mVec3_c *); ///< @unofficial

    static bool m_firstCalc;
};
