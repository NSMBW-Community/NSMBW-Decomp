#pragma once

#include <game/bases/d_actor.hpp>
#include <game/sLib/s_State.hpp>

/// @brief An implementation of dActor_c with state support.
/// @ingroup bases
class dActorState_c : public dActor_c {
public:
    dActorState_c(); ///< @copydoc dActor_c::dActor_c
    ~dActorState_c(); ///< @copydoc dActor_c::~dActor_c

    STATE_VIRTUAL_FUNC_DECLARE(dActorState_c, Gegneric); ///< An example state that does nothing.

    sFStateMgr_c<dActorState_c, sStateMethodUsr_FI_c> mStateMgr; ///< The state manager.

    /// @cond
    void dummy() {
        mStateMgr.initializeState();
        mStateMgr.finalizeState();
        mStateMgr.getOldStateID();
    }
    /// @endcond
};

/// @brief An implementation of dActor_c with multi state support.
/// @ingroup bases
class dActorMultiState_c : public dActor_c {
public:
    dActorMultiState_c() : mStateMgr(*this, sStateID::null) {} ///< @copydoc dActor_c::dActor_c
    ~dActorMultiState_c(); ///< @copydoc dActor_c::~dActor_c

    /// @brief Changes the actor's state to the given state.
    /// @param newState The state ID of the new state.
    virtual void changeState(const sStateIDIf_c &newState) { mStateMgr.changeState(newState); }
    STATE_VIRTUAL_FUNC_DECLARE(dActorMultiState_c, GegnericMulti); ///< An example state that does nothing.

    sFStateStateMgr_c<dActorMultiState_c, sStateMethodUsr_FI_c, sStateMethodUsr_FI_c> mStateMgr; ///< The state manager.

    /// @cond
    void dummy() {
        mStateMgr.initializeState();
        mStateMgr.finalizeState();
        mStateMgr.changeToSubState(*mStateMgr.getOldStateID());
    }
    /// @endcond
};
