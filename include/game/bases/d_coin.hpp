#pragma once
#include <types.h>

class dCoin_c {
public:
    static int execute();
protected:
    static s16 m_shapeAngle[3];
    static s16 m_h_shapeAngle[3];
    static u8 m_frame;
    static u8 m_ptn_no;
    static u8 m_h_ptn_no;
    static u8 m_h_ptn_time;
    static u8 m_b_ptn_no;
    static u8 m_b_ptn_time;
};
