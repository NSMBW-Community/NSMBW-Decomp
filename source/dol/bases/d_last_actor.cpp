#include <game/bases/d_last_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_scene.hpp>
#include <game/bases/d_movie.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/framework/f_profile.hpp>
#include <lib/egg/util/eggEffectManager.hpp>

BASE_PROFILE(LASTACTOR, dLastActor_c);

int dLastActor_c::create() {
    return SUCCEEDED;
}

int dLastActor_c::doDelete() {
    return SUCCEEDED;
}

int dLastActor_c::execute() {
    if (EGG::EffectManager::sEffectManager != nullptr) {
        if (dScene_c::m_nowScene == fProfile::WORLD_MAP) {
            EffectManager_c::calcGroupForWm();
        } else if (dScene_c::m_nowScene == fProfile::MOVIE) {
            if (!dScMovie_c::m_instance->mMovieEnded || dScMovie_c::m_instance->m_19D) {
                EffectManager_c::calcGroup();
            }
        } else {
            EffectManager_c::calcGroup();
        }
    }

    dGameCom::setGameStop();
    dActor_c::mExecStop = dActor_c::mExecStopReq;
    dActor_c::mDrawStop = dActor_c::mDrawStopReq;

    return SUCCEEDED;
}

int dLastActor_c::draw() {
    return SUCCEEDED;
}

dLastActor_c::~dLastActor_c() {}
