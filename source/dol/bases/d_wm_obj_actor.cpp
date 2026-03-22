#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_wm_obj_actor.hpp>
#include <game/bases/d_wm_lib.hpp>

int dWmObjActor_c::GetOpenStatus() {
    return dWmLib::GetOpenStatus(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

int dWmObjActor_c::GetClearStatus() {
    return dWmLib::GetClearStatus(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

int dWmObjActor_c::GetCurrentPlayResultStatus() {
    return dWmLib::GetCurrentPlayResultStatus(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo), ACTOR_PARAM(PathNode));
}

bool dWmObjActor_c::IsCourseOmoteClear() {
    return dWmLib::IsCourseOmoteClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseUraClear() {
    return dWmLib::IsCourseUraClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseOtasukeClear() {
    return dWmLib::IsCourseOtasukeClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseClear() {
    return dWmLib::IsCourseClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseFirstOmoteClear() {
    return dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo), ACTOR_PARAM(PathNode));
}

bool dWmObjActor_c::IsCourseFirstUraClear() {
    return dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo), ACTOR_PARAM(PathNode));
}

bool dWmObjActor_c::IsCourseFailed() {
    return dWmLib::IsCourseFailed(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo), ACTOR_PARAM(PathNode));
}

bool dWmObjActor_c::IsCourseFirstClear() {
    return dWmLib::IsCourseFirstClear(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseOtasukeClearSimple() {
    return dWmLib::IsCourseOtasukeClearSimple(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseOmoteClearSimple() {
    return dWmLib::IsCourseOmoteClearSimple(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseUraClearSimple() {
    return dWmLib::IsCourseUraClearSimple(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}

bool dWmObjActor_c::IsCourseUraOtasukeClearSimple() {
    return dWmLib::IsCourseUraOtasukeClearSimple(dScWMap_c::m_WorldNo, ACTOR_PARAM(CourseNo));
}
