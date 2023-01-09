#include <types.h>

#include <dol/s/s_StateMethodUsr_FI.hpp>

sStateMethodUsr_FI_c::sStateMethodUsr_FI_c(sStateIDChkIf_c &check, sStateFctIf_c &factory, const sStateIDIf_c &state) :
    sStateMethod_c(check, factory, state) {
}

int sStateMethodUsr_FI_c::initializeStateLocalMethod() {
    mpState = mpStateFct.build(*getNewStateID()); // Create new state holder with the next state ID
    mpState->initialize();
    return 1;
}

void sStateMethodUsr_FI_c::executeStateLocalMethod() {
    initializeStateMethod(); // To ensure we are in a valid state (note that only does something if !mIsValid)
    mpState->execute();
}

void sStateMethodUsr_FI_c::finalizeStateLocalMethod() {
    mpState->finalize();
    mpStateFct.dispose(mpState);
}

void sStateMethodUsr_FI_c::changeStateLocalMethod(const sStateIDIf_c &state) {
    finalizeStateMethod(); // Terminate the current state
    initializeStateMethod(); // And initialize the new state
}