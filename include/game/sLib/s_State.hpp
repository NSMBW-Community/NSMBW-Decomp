#pragma once
#include <game/sLib/s_FStateStateMgr.hpp>
#include <game/sLib/s_FStateMgr.hpp>
#include <game/sLib/s_StateMethodUsr_FI.hpp>
#include <game/sLib/s_FStateVirtualID.hpp>
/// @file

/// @brief Declares a state.
/// @param class The class name.
/// @param name The state name.
/// @hideinitializer
#define STATE_FUNC_DECLARE(class, name) \
    void initializeState_##name(); \
    void executeState_##name(); \
    void finalizeState_##name(); \
    static sFStateID_c<class> StateID_##name

/// @brief Declares a virtual state.
/// @param class The class name.
/// @param name The state name.
/// @hideinitializer
#define STATE_VIRTUAL_FUNC_DECLARE(class, name) \
    virtual void initializeState_##name(); \
    virtual void executeState_##name(); \
    virtual void finalizeState_##name(); \
    static sFStateVirtualID_c<class> StateID_##name

/// @brief Defines a state.
/// @param class The class name.
/// @param name The state name.
/// @hideinitializer
#define STATE_DEFINE(class, name) sFStateID_c<class> class::StateID_##name( \
    #class "::StateID_" #name, \
    &class::initializeState_##name, \
    &class::executeState_##name, \
    &class::finalizeState_##name)

/// @brief Defines a virtual state.
/// @param class The class name.
/// @param name The state name.
/// @hideinitializer
#define STATE_VIRTUAL_DEFINE(class, name) sFStateVirtualID_c<class> class::StateID_##name( \
    baseID_ ##name<sStateID_c> (), \
    #class "::StateID_" #name, \
    &class::initializeState_##name, \
    &class::executeState_##name, \
    &class::finalizeState_##name)

#define STATE_BASE_VIRTUAL_DEFINE(class, name) \
    template <typename T> \
    const sStateIDIf_c &baseID_##name() { \
        return T::StateID_##name; \
    } \
    template <> \
    const sStateIDIf_c &baseID_##name<sStateID_c>() { \
        return sStateID::null; \
    }

