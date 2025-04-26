#pragma once
#include <game/sLib/s_StateInterfaces.hpp>
#include <game/sLib/s_StateIDChk.hpp>

/// @brief The interface for state managers.
/// @details A state manager handles execution of and transitioning between state IDs.
class sStateMgrIf_c {
public:
    virtual ~sStateMgrIf_c() {}
    virtual void initializeState() = 0; ///<  Initializes the current state.
    virtual void executeState() = 0; ///<  Executes the current state.
    virtual void finalizeState() = 0; ///<  Prepares the current state for termination.
    virtual void changeState(const sStateIDIf_c &newStateID) = 0; ///<  Transitions to a new state ID.
    virtual void refreshState() = 0; ///<  Marks the current state to be executed again.
    virtual sStateIf_c *getState() const = 0; ///<  Gets the state holder.
    virtual const sStateIDIf_c *getNewStateID() const = 0; ///<  Gets the next state ID.
    virtual const sStateIDIf_c *getStateID() const = 0; ///<  Gets the current state ID.
    virtual const sStateIDIf_c *getOldStateID() const = 0; ///<  Gets the previous state ID.
};

/**
 * @brief An implementation of sStateMgrIf_c.
 *
 * @tparam T The parent class for this state manager.
 * @tparam Method The state method handler to use.
 * @tparam Factory The state factory to use.
 * @tparam Check The state ID checker to use.
 * @ingroup state
 */
template <class T, class Method, template <class> class Factory, class Check>
class sStateMgr_c : public sStateMgrIf_c {
public:
    sStateMgr_c(T &owner, const sStateIDIf_c &initialState) :
        mFactory(owner),
        mMethod(mCheck, mFactory, initialState) {}

    virtual void initializeState() { mMethod.initializeStateMethod(); }
    virtual void executeState() { mMethod.executeStateMethod(); }
    virtual void finalizeState() { mMethod.finalizeStateMethod(); }

    virtual void changeState(const sStateIDIf_c &newState) { mMethod.changeStateMethod(newState); }

    virtual void refreshState() { mMethod.refreshStateMethod(); }

    virtual sStateIf_c *getState() const { return mMethod.getState(); }
    virtual const sStateIDIf_c *getNewStateID() const { return mMethod.getNewStateID(); }
    virtual const sStateIDIf_c *getStateID() const { return mMethod.getStateID(); }
    virtual const sStateIDIf_c *getOldStateID() const { return mMethod.getOldStateID(); }

private:
    Check mCheck;
    Factory<T> mFactory;
    Method mMethod;
};
