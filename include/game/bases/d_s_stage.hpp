#pragma once
#include <game/bases/d_scene.hpp>
#include <game/mLib/m_vec.hpp>

class dScStage_c : public dScene_c {
public:

    /// @brief The possible stage loop types.
    enum LOOP_TYPE_e {
        LOOP_NONE, ///< No stage looping occurs.
        LOOP_EDGES, ///< The stage loops around on the zone edges. Only works for specific zone sizes.
        LOOP_SECTION, ///< The stage loops in specific sections.
        LOOP_COUNT,
    };

    char pad[0x119a];
    u8 mCurrCourse;

    static float getLoopPosX(float x);
    static int m_loopType;

    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.
    static bool m_KoopaJrEscape;

    typedef void (*changePosFunc)(mVec3_c *);
    static void setChangePosFunc(int);

    static changePosFunc changePos;
    static dScStage_c *m_instance;
};
