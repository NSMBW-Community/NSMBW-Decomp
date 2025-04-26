#pragma once
#include <game/sLib/s_FStateID.hpp>
#include <game/sLib/s_StateID.hpp>

template <typename T>
class sFStateVirtualID_c : public sFStateID_c<T> {
public:
    typedef void (T::*stateFunc)();

    sFStateVirtualID_c(
        const sStateIDIf_c &superState, const char *name,
        stateFunc initialize, stateFunc execute, stateFunc finalize
    ) :
        sFStateID_c<T>(name, initialize, execute, finalize),
        mSuperState(superState) {}

    virtual unsigned int number() const {
        return superID()->numberBase();
    }

    const sFStateVirtualID_c<T> *superID() const {
        if (!mSuperState.isNull()) {
            return ((const sFStateVirtualID_c<T> &) mSuperState).superID();
        }
        return this;
    }

    unsigned int numberBase() const {
        return sStateID_c::number();
    }

private:
    const sStateIDIf_c &mSuperState;
};