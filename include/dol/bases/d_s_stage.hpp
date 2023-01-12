#pragma once
#include <dol/bases/d_scene.hpp>

class dScStage_c : public dScene_c {
public:
    static bool m_isCourseOut; ///< Whether the game is transitioning from a stage scene to a non-stage scene.
};
