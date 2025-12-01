#pragma once
#include <game/mLib/m_angle.hpp>
#include <types.h>

class dCoin_c {
public:
    static int execute();
private:
    static mAng3_c m_shapeAngle;
    static mAng3_c m_h_shapeAngle;
    static u8 m_frame;
    static u8 m_ptn_no;
    static u8 m_h_ptn_no;
    static u8 m_h_ptn_time;
    static u8 m_b_ptn_no;
    static u8 m_b_ptn_time;
};
