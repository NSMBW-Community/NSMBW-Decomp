#pragma once
#include "game/framework/f_base.hpp"
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_actor.hpp>

class dAwa_c : public dActor_c {
public:
    typedef void (dAwa_c::*CalcSpeed)();

    dAwa_c() : mCalcSpeedFn(), mLifeTimer(60) {}

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();
    virtual ~dAwa_c() {}

    void createModel();
    void setAnim();
    void calc();

    void calcSpeedUp();
    void calcSpeedDown();
    void calcSpeedLeft();
    void calcSpeedRight();

    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mResFile;
    m3d::mdl_c mBubbleModel;
    m3d::anmChr_c mBubbleAnim;
    CalcSpeed mCalcSpeedFn;
    int mLifeTimer;

    ACTOR_PARAM_CONFIG(Direction, 0, 2);
};
