#pragma once
#include <constants/game_constants.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_wm_csv_data.hpp>

namespace dWmLib {
    enum Direction3D_e {
        DIR3D_UP,
        DIR3D_DOWN,
        DIR3D_FRONT,
        DIR3D_BACK,
        DIR3D_LEFT,
        DIR3D_RIGHT
    };

    /// @unofficial
    enum CourseType_e {
        COURSE_TYPE_NORMAL,
        COURSE_TYPE_GHOST,
        COURSE_TYPE_TOWER,
        COURSE_TYPE_CASTLE,
        COURSE_TYPE_KINOKO,
        COURSE_TYPE_JUNCTION,
        COURSE_TYPE_CANNON,
        COURSE_TYPE_STAGE_37,
        COURSE_TYPE_KOOPA_SHIP,
        COURSE_TYPE_KINOKO_START,
        COURSE_TYPE_PEACH_CASTLE,
        COURSE_TYPE_INVALID
    };

    /// @unofficial
    enum PointType_e {
        POINT_TYPE_INTERSECTION,
        POINT_TYPE_PATH,
        POINT_TYPE_START_NODE,
        POINT_TYPE_REGULAR_COURSE,
        POINT_TYPE_OTHER
    };

    struct ForceInCourseList_t {
        ~ForceInCourseList_t() {}

        int nodeWorld;
        const char *nodeName;
        int world;
        int level;
        int entrance;
        const char *levelNode;
        mVec3_c nodePos;
    };

    int GetCourseTypeFromCourseNo(int courseNo);
    bool isKoopaShipAnchor();
    u8 getStartPointKinokoHouseKindNum();
    bool isStartPointKinokoHouseStar();
    bool isStartPointKinokoHouseRed();

    int GetOpenStatus(int world, int course);
    int GetClearStatus(int world, int course);
    int GetCurrentPlayResultStatus();
    int GetCurrentPlayResultStatus(int world, int course, int pathNode);

    bool IsCourseOmoteClear(int world, int course);
    bool IsCourseUraClear(int world, int course);
    bool IsCourseOtasukeClear(int world, int course);
    bool IsCourseClear(int world, int course);
    bool IsCourseFirstOmoteClear(int world, int course, int pathNode);
    bool IsCourseFirstUraClear(int world, int course, int pathNode);
    bool IsCourseFailed(int world, int course, int pathNode);
    bool IsCourseFirstClear(int world, int course);
    bool IsCourseOtasukeClearSimple(int world, int course);
    bool IsCourseOmoteClearSimple(int world, int course);
    bool IsCourseUraClearSimple(int world, int course);
    bool IsCourseUraOtasukeClearSimple(int world, int course);

    int getPointDir(const mVec3_c &v1, const mVec3_c &v2);
    int getEnemyRevivalCount(int, int);

    static ForceInCourseList_t sc_ForceList[] = {
        {WORLD_7, "F7C0", WORLD_7, dCsvData_c::c_CASTLE_ID, 4, "W7C0", mVec3_c(2160, -30, -478)}
    };

    static int c_StartPointKinokoHouseID = dCsvData_c::c_START_ID;
};
