#pragma once

#include <game/bases/d_scene.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

class dScStage_c : public dScene_c {
public:

    /// @brief The possible stage loop types.
    enum LOOP_TYPE_e {
        LOOP_NONE, ///< No stage looping occurs.
        LOOP_EDGES, ///< The stage loops around on the zone edges. Only works for specific zone sizes.
        LOOP_SECTION, ///< The stage loops in specific sections.
        LOOP_COUNT,
    };

    u8 mPad1[0x1198];
    u8 mCurrWorld;
    u8 mCurrCourse;
    u8 mCurrFile;
    u8 mPad2[0x2];
    u8 m_1211;

    static float getLoopPosX(float x);
    static int m_loopType;
    static int mCollectionCoin[STAR_COIN_COUNT];

    static u32 m_exeFrame;

    static const char mCdArcName[];

    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.
    static bool m_KoopaJrEscape;

    typedef void (*changePosFunc)(mVec3_c *);
    static void setChangePosFunc(int);

    static void setTitleReplayRandomTable();

    static changePosFunc changePos;
    static dScStage_c *m_instance;
};
