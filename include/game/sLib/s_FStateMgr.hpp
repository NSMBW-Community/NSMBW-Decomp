#pragma once
#include <game/sLib/s_FStateID.hpp>
#include <game/sLib/s_StateMgr.hpp>
#include <game/sLib/s_FState.hpp>
#include <game/sLib/s_FStateFct.hpp>
#include <game/sLib/s_StateIDChk.hpp>

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
