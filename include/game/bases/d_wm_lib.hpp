#include <game/mLib/m_vec.hpp>

namespace CourseNames {
    extern int castleCourseNo;
    extern int rescueCourseNo;
}

struct ForceListElement {
    int a;
    char *b;
    int c;
    int d;
    int e;
    char *f;
    mVec3_c vec;

    ~ForceListElement();
};

namespace dWmLib {
    static ForceListElement sc_ForceList[1] = {
        {6, "F7C0", 6, CourseNames::castleCourseNo, 4, "W7C0", nw4r::math::VEC3(2160, -30, -478)}
    };
    static int someCourseNo = CourseNames::rescueCourseNo;
};
