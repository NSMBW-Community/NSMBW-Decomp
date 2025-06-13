#pragma once
#include <game/bases/d_scene.hpp>
#include <game/mLib/m_vec.hpp>

class dScStage_c : public dScene_c {
public:
    char pad[0x1198];
    u8 mCurrWorld;
    u8 mCurrLevel;
    u8 mCurrFile;
    u8 mCurrAreaNo;
    u8 mCurrLayer;

    static float getLoopPosX(float x);
    static int m_loopType;

    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.

    typedef void (*changePosFunc)(mVec3_c *);
    static void setChangePosFunc(int);

    static int m_gameMode;
    static bool m_isStaffCredit;
    static changePosFunc changePos;
    static dScStage_c *m_instance;
};
