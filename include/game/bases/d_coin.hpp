#pragma once
#include <types.h>
#include <game/mLib/m_angle.hpp>

/// @brief Controls the rotating animation for coins.
/// @details Also contains unused logic for controlling brick block and question block animations.
/// @ingroup bases
class dCoin_c {
public:
    static int execute(); ///< Executes coin rotation.

    static mAng3_c getShapeAngle() { return m_h_shapeAngle; }
private:
    static const int COIN_FRAME_COUNT = 64; ///< The coin rotation animation frame count.
    static mAng3_c m_shapeAngle; ///< The rotation to be applied to every coin.
    static mAng3_c m_h_shapeAngle; ///< @unused
    static u8 m_frame; ///< The coin rotation animation frame.
    static u8 m_ptn_no; ///< The current animated tile frame for coins. @unused
    static u8 m_h_ptn_no; ///< The current animated tile frame for question blocks. @unused
    static u8 m_h_ptn_time; ///< The remaining time for the current question block tile frame. @unused
    static u8 m_b_ptn_no; ///< The current animated tile frame for brick blocks. @unused
    static u8 m_b_ptn_time; ///< The remaining time for the current brick block tile frame. @unused
};
