#pragma once

#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_mtx.hpp>
#include <lib/egg/core/eggEffect.h>

class mEf {
public:
    static void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);

    class effect_c : public EGG::Effect {
    public:
        virtual void createEffect(const char *, int);
        virtual void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);
        virtual void createEffect(const char *, unsigned long, const mMtx_c *);

        u8 mPad[0x92];
    };

    class levelEffect_c : public effect_c {
    public:
        levelEffect_c() : m_114(0), m_118(0), m_11c(0), m_11d(0), m_120(0), m_124(0) {}
        virtual ~levelEffect_c() { cleanup(); }

        virtual void createEffect(const char *, int);
        virtual void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);
        virtual void createEffect(const char *, unsigned long, const mMtx_c *);

        void cleanup();

        u32 m_114, m_118;
        u8 m_11c, m_11d;
        u32 m_120, m_124;
    };
};
