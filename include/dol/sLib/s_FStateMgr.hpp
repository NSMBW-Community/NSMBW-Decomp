#pragma once
#include <dol/sLib/s_FStateID.hpp>
#include <dol/sLib/s_StateMgr.hpp>
#include <dol/sLib/s_FState.hpp>
#include <dol/sLib/s_FStateFct.hpp>

/// @brief A wrapper for sStateMgr_c that uses sFStateFct_c and sStateIDChk_c.
/// @tparam T The class that this state belongs to.
/// @tparam Method The method to use for the state manager.
/// @ingroup state
template <class T, class Method>
class sFStateMgr_c : public sStateMgr_c<T, Method, sFStateFct_c, sStateIDChk_c> {
public:
    sFStateMgr_c(T &owner, const sStateIDIf_c &initializeState) :
        sStateMgr_c<T, Method, sFStateFct_c, sStateIDChk_c>(owner, initializeState) {}
};
