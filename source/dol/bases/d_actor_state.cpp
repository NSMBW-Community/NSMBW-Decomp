#include <game/bases/d_actor_state.hpp>

STATE_BASE_VIRTUAL_DEFINE(dActorState_c, Gegneric);
STATE_VIRTUAL_DEFINE(dActorState_c, Gegneric);

dActorState_c::dActorState_c() : mStateMgr(*this, sStateID::null) {}

dActorState_c::~dActorState_c() {}

void dActorState_c::initializeState_Gegneric() {}
void dActorState_c::finalizeState_Gegneric() {}
void dActorState_c::executeState_Gegneric() {}

STATE_BASE_VIRTUAL_DEFINE(dActorMultiState_c, GegnericMulti);
STATE_VIRTUAL_DEFINE(dActorMultiState_c, GegnericMulti);

dActorMultiState_c::~dActorMultiState_c() {}

void dActorMultiState_c::initializeState_GegnericMulti() {}
void dActorMultiState_c::finalizeState_GegnericMulti() {}
void dActorMultiState_c::executeState_GegnericMulti() {}
