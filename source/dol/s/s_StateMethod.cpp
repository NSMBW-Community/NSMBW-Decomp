#include <types.h>

#include <dol/s/s_StateInterfaces.hpp>
#include <dol/s/s_StateMethod.hpp>
#include <dol/s/s_StateID.hpp>

sStateMethod_c::sStateMethod_c(sStateIDChkIf_c &checker, sStateFctIf_c &factory, const sStateIDIf_c &initialState) :
    mpStateChk(checker),
    mpStateFct(factory),
    mInitFinalizeLock(false),
    mExecutionLock(false),
    mIsValid(false),
    mStateChanged(false),
    mRefreshStateMethod(false),
    mpNewStateID((sStateIDIf_c *) &initialState),
    mpOldStateID(&sStateID::null),
    mpStateID((sStateIDIf_c *) &initialState),
    mpState(nullptr) {
}

sStateMethod_c::~sStateMethod_c() {}

void sStateMethod_c::initializeStateMethod() {
    if (!mpNewStateID->isNull() && !mInitFinalizeLock && !mIsValid) {
        mInitFinalizeLock = true;

        mpStateID = mpNewStateID;
        int ret = initializeStateLocalMethod();
        if (ret != 0) {
            mIsValid = true;
        } else {
            mIsValid = false;
        }

        mInitFinalizeLock = false;
    }
}

void sStateMethod_c::executeStateMethod() {
    if (!mExecutionLock) {
        do {
            mRefreshStateMethod = false;
            // We only want to execute if we have a valid next state
            if (!mpNewStateID->isNull()) {
                mExecutionLock = true;
                executeStateLocalMethod();
                mExecutionLock = false;
            }
        } while (mRefreshStateMethod && mStateChanged);
    }
}

void sStateMethod_c::finalizeStateMethod() {
    if (!mpNewStateID->isNull() && mIsValid && !mInitFinalizeLock) {
        mInitFinalizeLock = true;

        mpOldStateID = mpStateID;
        finalizeStateLocalMethod();
        mIsValid = false;

        mInitFinalizeLock = false;
    }
}

void sStateMethod_c::changeStateMethod(const sStateIDIf_c &state) {
    if (!state.isNull()) {
        mpNewStateID = (sStateIDIf_c *) &state;
        changeStateLocalMethod(state);
        mStateChanged = true;
    }
}
