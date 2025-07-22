#include <constants/game_constants.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_wm_csv_data.hpp>

namespace dWmLib {

    struct ForceInCourseList_t {
        int nodeWorld;
        const char *nodeName;
        int world;
        int level;
        int entrance;
        const char *levelNode;
        mVec3_c nodePos;

        ~ForceInCourseList_t() {}
    };

    static ForceInCourseList_t sc_ForceList[] = {
        {WORLD_7, "F7C0", WORLD_7, dCsvData_c::c_CASTLE_ID, 4, "W7C0", mVec3_c(2160, -30, -478)}
    };

    /// @unofficial
    static int someCourseNo = dCsvData_c::c_START_ID;
};
