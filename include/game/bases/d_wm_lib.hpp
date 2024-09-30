#include <constants/game_constants.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_wm_csv_data.hpp>

/// @unofficial
struct ForceListElement {
    int nodeWorld;
    char *nodeName;
    int world;
    int level;
    int entrance;
    char *levelNode;
    mVec3_c nodePos;

    ~ForceListElement();
};

namespace dWmLib {
    static ForceListElement sc_ForceList[] = {
        {WORLD_7, "F7C0", WORLD_7, dCsvData_c::c_CASTLE_ID, 4, "W7C0", nw4r::math::VEC3(2160, -30, -478)}
    };

    /// @unofficial
    static int someCourseNo = dCsvData_c::c_START_ID;
};
