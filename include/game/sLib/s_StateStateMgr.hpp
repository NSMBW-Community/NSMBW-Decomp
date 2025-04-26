#pragma once
#include <game/sLib/s_StateMgr.hpp>

class sStateStateMgrIf_c : public sStateMgrIf_c {
public:
    virtual void changeToSubState(const sStateIDIf_c &newState) = 0;
    virtual void returnState() = 0;
    virtual bool isSubState() const = 0;
    virtual const sStateIDIf_c *getMainStateID() const = 0;
};

template <class T, template <class, class> class Manager, class Method1, class Method2>
class sStateStateMgr_c : public sStateStateMgrIf_c {
public:
    sStateStateMgr_c(T &owner, const sStateIDIf_c &initialState) :
        mainMgr(owner, initialState),
        subMgr(owner, initialState),
        currentMgr(&mainMgr) {}

    virtual void initializeState() { currentMgr->initializeState(); }
    virtual void executeState() { currentMgr->executeState(); }
    virtual void finalizeState() {
        if (isSubState()) {
            returnState();
        } else {
            currentMgr->finalizeState();
        }
    }

    virtual bool isSubState() const { return currentMgr == &subMgr; }

    virtual void returnState() {
        if (isSubState()) {
            currentMgr->finalizeState();
            currentMgr = &mainMgr;
        }
    }

    virtual const sStateIDIf_c *getOldStateID() const { return currentMgr->getOldStateID(); }

    virtual void refreshState() { currentMgr->refreshState(); }

    virtual void changeToSubState(const sStateIDIf_c &newState) {
        currentMgr = &subMgr;
        currentMgr->changeState(newState);
    }

    virtual void changeState(const sStateIDIf_c &newState) { currentMgr->changeState(newState); }
    virtual sStateIf_c *getState() const { return currentMgr->getState(); }
    virtual const sStateIDIf_c *getNewStateID() const { return currentMgr->getNewStateID(); }
    virtual const sStateIDIf_c *getStateID() const { return currentMgr->getStateID(); }
    virtual const sStateIDIf_c *getMainStateID() const { return mainMgr.getStateID(); }

    Manager<T, Method1> mainMgr;
    Manager<T, Method2> subMgr;
    sStateMgrIf_c *currentMgr;
};
