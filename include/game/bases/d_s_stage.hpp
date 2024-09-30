#pragma once
#include <game/bases/d_scene.hpp>

class dScStage_c : public dScene_c {
public:
    static float getLoopPosX(float x);
    static int m_loopType;
    
    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.
};
