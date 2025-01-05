#pragma once

#include <game/bases/d_actor.hpp>
#include <game/sLib/s_State.hpp>

class dActorState_c : public dActor_c {
public:
    dActorState_c();
    ~dActorState_c();

    STATE_VIRTUAL_FUNC_DECLARE(dActorState_c, Gegneric);

    sFStateMgr_c<dActorState_c, sStateMethodUsr_FI_c> mStateMgr;
    void dummy() {
        mStateMgr.initializeState();
        mStateMgr.finalizeState();
        mStateMgr.getOldStateID();
    }
};

class dActorMultiState_c : public dActor_c {
public:
    dActorMultiState_c() : mStateMgr(*this, sStateID::null) {}
    ~dActorMultiState_c();

    virtual void changeState(const sStateIDIf_c &newState) { mStateMgr.changeState(newState); }
    STATE_VIRTUAL_FUNC_DECLARE(dActorMultiState_c, GegnericMulti);

    sFStateStateMgr_c<dActorMultiState_c, sStateMethodUsr_FI_c, sStateMethodUsr_FI_c> mStateMgr;
    void dummy() {
        mStateMgr.initializeState();
        mStateMgr.finalizeState();
        mStateMgr.changeToSubState(*mStateMgr.getOldStateID());
    }
};
