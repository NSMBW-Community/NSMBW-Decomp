#pragma once
#include <dol/s/s_StateInterfaces.hpp>

/// @brief A class that handles state execution and transition.
/// @details [Presumably, sStateMethod_c actually means "methods for state interaction", or something like that.]
class sStateMethod_c {
public:
    sStateMethod_c(sStateIDChkIf_c &checker, sStateFctIf_c &factory, const sStateIDIf_c &initialState);
    virtual ~sStateMethod_c();

    virtual void initializeStateMethod(); ///< @copydoc sStateMgrIf_c::initializeState
    virtual void executeStateMethod(); ///< @copydoc sStateMgrIf_c::executeState
    virtual void finalizeStateMethod(); ///< @copydoc sStateMgrIf_c::finalizeState
    virtual void changeStateMethod(const sStateIDIf_c &);  ///< @copydoc sStateMgrIf_c::changeState
    virtual void refreshStateMethod() { mRefreshStateMethod = true; } ///< @copydoc sStateMgrIf_c::refreshState
    virtual sStateIf_c *getState() const { return mpState; } ///< @copydoc sStateMgrIf_c::getState
    virtual sStateIDIf_c *getNewStateID() const { return mpNewStateID; } ///< @copydoc sStateMgrIf_c::getNewStateID
    virtual sStateIDIf_c *getStateID() const { return mpStateID; } ///< @copydoc sStateMgrIf_c::getStateID
    virtual sStateIDIf_c *getOldStateID() const { return mpOldStateID; } ///< @copydoc sStateMgrIf_c::getOldStateID

    virtual int initializeStateLocalMethod() = 0; ///< Performs the actual state initialization.
    virtual void executeStateLocalMethod() = 0; ///< Performs the actual state execution.
    virtual void finalizeStateLocalMethod() = 0; ///< Performs the actual state termination.
    virtual void changeStateLocalMethod(const sStateIDIf_c &) = 0; ///< Performs the actual state transition.

protected:
    sStateIDChkIf_c &mpStateChk; ///< The state checker to use. [Unused.]
    sStateFctIf_c &mpStateFct; ///< The state factory which produces the state holder.

    bool mInitFinalizeLock; ///< A lock to ensure ::initializeStateMethod and ::finalizeStateMethod are not called recursively.
    bool mExecutionLock; ///< A lock to ensure ::executeStateMethod is not called recursively.
    bool mIsValid; ///< If the state holder contains a valid state ID.
    bool mStateChanged; ///< If the current state has changed during execution.
    bool mRefreshStateMethod; ///< True, if after a state transition, the state should be executed again.

    sStateIDIf_c *mpNewStateID; ///< The next state ID.
    sStateIDIf_c *mpOldStateID; ///< The previous state ID.
    sStateIDIf_c *mpStateID; ///< The current state ID.
    
    sStateIf_c *mpState; ///< The holder of the current state.
};
