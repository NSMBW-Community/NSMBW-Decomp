#pragma once
#include <lib/egg/eggEffect.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_mtx.hpp>

namespace mEf {

class effect_c : public EGG::Effect {
public:
    effect_c() {}

    virtual void reset();
    virtual void createEffect(const char *, int);
    virtual void createEffect(const char *, ulong, const mVec3_c *, const mAng3_c *, const mVec3_c *);
    virtual void createEffect(const char *, ulong, const mMtx_c *);
    virtual void vfa8();
    virtual void vfac();
    virtual void follow(const mVec3_c *, const mAng3_c *, const mVec3_c *);
    virtual void follow(const mMtx_c *);

    u8 mPad[0x92];
    mAng mAng;
};

class levelEffect_c : public effect_c {
public:
    levelEffect_c() : m_114(0), m_118(0), m_11c(0), m_11d(0), m_120(0), m_124(0) {}
    virtual ~levelEffect_c() { cleanup(); }

    virtual void fade();
    virtual void kill();
    virtual void update();
    virtual void createEffect(const char *, int);
    virtual void createEffect(const char *, ulong, const mVec3_c *, const mAng3_c *, const mVec3_c *);
    virtual void createEffect(const char *, ulong, const mMtx_c *);
    virtual void vfa8();
    virtual void vfac();
    virtual void follow(const mVec3_c *, const mAng3_c *, const mVec3_c *);
    virtual void follow(const mMtx_c *);
    virtual void isActive();

    void cleanup();

    u32 m_114, m_118;
    u8 m_11c, m_11d;
    u32 m_120, m_124;
};

void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);

}; // namespace mEf
