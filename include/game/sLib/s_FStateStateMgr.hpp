#pragma once
#include <game/sLib/s_StateStateMgr.hpp>
#include <game/sLib/s_FStateMgr.hpp>
#include <game/sLib/s_StateMgr.hpp>
#include <game/sLib/s_FState.hpp>
#include <game/sLib/s_StateIDChk.hpp>
#include <game/sLib/s_FStateFct.hpp>
#include <game/sLib/s_FStateFct.hpp>

template <class T, class Method1, class Method2>
class sFStateStateMgr_c : public sStateStateMgr_c<T, sFStateMgr_c, Method1, Method2> {
public:
    sFStateStateMgr_c(T &owner, const sStateIDIf_c &initializeState) :
        sStateStateMgr_c<T, sFStateMgr_c, Method1, Method2>(owner, initializeState) {}
};
