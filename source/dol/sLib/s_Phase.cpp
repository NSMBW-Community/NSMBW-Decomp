#include <game/sLib/s_Phase.hpp>

sPhase_c::sPhase_c(phaseMethod *methodList, int count) :
    mpMethodList(methodList),
    mPhaseLength(count),
    mCurrMethod(0) {
}

sPhase_c::METHOD_RESULT_e sPhase_c::callMethod(void *thisPtr) {
    if (mCurrMethod >= mPhaseLength) {
        return DONE;
    }

    METHOD_RESULT_e result = OK;
    while (result == OK) {
        result = (mpMethodList[mCurrMethod])(thisPtr);

        if (result == OK) {
            // Go to the next method
            mCurrMethod++;
            if (mCurrMethod >= mPhaseLength) {
                return DONE;
            }
        }
    }

    return result;
}
