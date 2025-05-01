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

    char pad[0x1198];
    u8 m_120c;
    u8 m_120d;
    u8 mCurrCourse;
    u8 mCurrArea;
    u8 mCurrWorld;

    static float getLoopPosX(float x);
    static int m_loopType;
    static u32 m_exeFrame;

    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.

    typedef void (*changePosFunc)(mVec3_c *);
    static void setChangePosFunc(int);

    static changePosFunc changePos;
    static dScStage_c *m_instance;
};
