#pragma once
#include <constants/game_constants.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_wm_csv_data.hpp>

/// @unofficial
struct ForceListElement {
    int nodeWorld;
    const char *nodeName;
    int world;
    int level;
    int entrance;
    const char *levelNode;
    mVec3_c nodePos;

    ~ForceListElement() {}
};

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

    int GetCourseTypeFromCourseNo(int courseNo);
    bool isKoopaShipAnchor();
    u8 getStartPointKinokoHouseKindNum();
    bool isStartPointKinokoHouseStar();
    bool isStartPointKinokoHouseRed();
    bool IsCourseClear(int world, int course);
    int getPointDir(const mVec3_c &v1, const mVec3_c &v2);
    int GetCurrentPlayResultStatus();
    int getEnemyRevivalCount(int, int);

    /// @unofficial
    static ForceListElement sc_ForceList[] = {
        {WORLD_7, "F7C0", WORLD_7, dCsvData_c::c_CASTLE_ID, 4, "W7C0", mVec3_c(2160, -30, -478)}
    };

    /// @unofficial
    static int someCourseNo = dCsvData_c::c_START_ID;
};
