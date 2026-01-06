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

    enum EXIT_MODE_e {
        EXIT_0,
        EXIT_1,
        EXIT_2,
        EXIT_3
    };

    char pad[0x1198];
    u8 mCurrWorld;
    u8 mCurrLevel;
    u8 mCurrFile;
    u8 mCurrAreaNo;
    u8 mCurrLayer;

    static NOINLINE EXIT_MODE_e getExitMode() { return m_exitMode; }

    static float getLoopPosX(float x);
    static int m_loopType;
    static int mCollectionCoin[STAR_COIN_COUNT];

    static const char mCdArcName[];

    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.
    static bool m_KoopaJrEscape;

    typedef void (*changePosFunc)(mVec3_c *);
    static void setChangePosFunc(int);

    static void setTitleReplayRandomTable();

    static int m_gameMode;
    static int m_miniGame;
    static EXIT_MODE_e m_exitMode;
    static bool m_isStaffCredit;
    static changePosFunc changePos;
    static dScStage_c *m_instance;
};
