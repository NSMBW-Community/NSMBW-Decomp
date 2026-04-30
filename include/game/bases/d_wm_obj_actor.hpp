#pragma once
#include <game/bases/d_wm_demo_actor.hpp>

/**
 * @brief The minimum required implementation for world map objects.
 * @paramtable
 * @details World map objects are event-driven decorative objects, which can be associated
 * with a specific level or path node.
 * @hint{If the object is not attached to a level or path node, the corresponding parameters
 * can be overridden.}
 * @ingroup bases
*/
class dWmObjActor_c : public dWmDemoActor_c {
public:
    enum PROC_TYPE_e {
        PROC_TYPE_EXEC,
        PROC_COUNT
    };

    dWmObjActor_c() : mResNodeIdx(-1) {} ///< Constructs a new object.
    ~dWmObjActor_c() {} ///< Destroys the object.

    virtual int GetActorType() { return ACTOR_MAP_OBJECT; }

    virtual void vf74() {} ///< @unofficial
    virtual bool vf78() { return false; } ///< @unofficial

    int GetOpenStatus(); ///< @copybrief dWmLib::GetOpenStatus
    int GetClearStatus(); ///< @copybrief dWmLib::GetClearStatus
    int GetCurrentPlayResultStatus(); ///< @copybrief dWmLib::GetCurrentPlayResultStatus

    bool IsCourseOmoteClear(); ///< @copybrief dWmLib::IsCourseOmoteClear
    bool IsCourseUraClear(); ///< @copybrief dWmLib::IsCourseUraClear
    bool IsCourseOtasukeClear(); ///< @copybrief dWmLib::IsCourseOtasukeClear
    bool IsCourseClear(); ///< @copybrief dWmLib::IsCourseClear
    bool IsCourseFirstOmoteClear(); ///< @copybrief dWmLib::IsCourseFirstOmoteClear
    bool IsCourseFirstUraClear(); ///< @copybrief dWmLib::IsCourseFirstUraClear
    bool IsCourseFailed(); ///< @copybrief dWmLib::IsCourseFailed
    bool IsCourseFirstClear(); ///< @copybrief dWmLib::IsCourseFirstClear
    bool IsCourseOtasukeClearSimple(); ///< @copybrief dWmLib::IsCourseOtasukeClearSimple
    bool IsCourseOmoteClearSimple(); ///< @copybrief dWmLib::IsCourseOmoteClearSimple
    bool IsCourseUraClearSimple(); ///< @copybrief dWmLib::IsCourseUraClearSimple
    bool IsCourseUraOtasukeClearSimple(); ///< @copybrief dWmLib::IsCourseUraOtasukeClearSimple

    int mResNodeIdx; ///< The index of the model's root resource node.

    ACTOR_PARAM_CONFIG(CourseNo, 0, 8); ///< The level number associated to the object.
    ACTOR_PARAM_CONFIG(PathNode, 8, 8); ///< The path node associated to the object.
};
