#pragma once
#include <dol/s/s_StateMethod.hpp>

/// @brief An extension to sStateMethod_c that implements the remaining abstract methods.
/// @details [Name might mean "Functionality implementation".]
class sStateMethodUsr_FI_c : public sStateMethod_c {
public:
    sStateMethodUsr_FI_c(sStateIDChkIf_c &, sStateFctIf_c &, const sStateIDIf_c &);
    virtual ~sStateMethodUsr_FI_c() {}

    virtual int initializeStateLocalMethod();
    virtual void executeStateLocalMethod();
    virtual void finalizeStateLocalMethod();
    virtual void changeStateLocalMethod(const sStateIDIf_c &);
};
