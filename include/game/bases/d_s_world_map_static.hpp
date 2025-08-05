#pragma once
#include <types.h>

class dScWMap_c {
public:
    /// @todo Fill out this enum.
    enum CourseType_e {
        COURSE_TYPE_NO_STAR_COINS = 2,
        COURSE_TYPE_KINOKO_HOUSE_1UP = 0x80,
        COURSE_TYPE_KINOKO_HOUSE_STAR = 0x200
    };

    static u8 getWorldNo() NOINLINE { return m_WorldNo; }

    static bool IsCourseType(int, int, CourseType_e);

    static u8 m_WorldNo;
    static u8 m_PrevWorldNo;
    static u8 m_SceneNo;
    static u8 m_PrevSceneNo;
};
