#pragma once
#include <game/bases/d_scene.hpp>

class dScStage_c : public dScene_c {
public:
    char pad[0x119a];
    u8 currCourse;

    static float getLoopPosX(float x);
    static int m_loopType;

    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.

    typedef void (*changePosFunc)(mVec3_c *);
    static void setChangePosFunc(int);

    static changePosFunc changePos;
    static dScStage_c *m_instance;
};
