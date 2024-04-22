#include <game/mLib/m_vec.hpp>
#include <game/bases/d_wm_csv_data.hpp>

/// @unofficial
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
        {6, "F7C0", 6, dCsvData_c::c_CASTLE_ID, 4, "W7C0", nw4r::math::VEC3(2160, -30, -478)}
    };

    /// @unofficial
    static int someCourseNo = dCsvData_c::c_START_ID;
};
