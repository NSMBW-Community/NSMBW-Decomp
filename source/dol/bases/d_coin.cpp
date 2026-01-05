#include "game/mLib/m_angle.hpp"
#include <game/bases/d_coin.hpp>
#include <types.h>

mAng3_c dCoin_c::m_shapeAngle;
mAng3_c dCoin_c::m_h_shapeAngle;
u8 dCoin_c::m_frame;
u8 dCoin_c::m_ptn_no;
u8 dCoin_c::m_h_ptn_no;
u8 dCoin_c::m_h_ptn_time;
u8 dCoin_c::m_b_ptn_no;
u8 dCoin_c::m_b_ptn_time;

int dCoin_c::execute() {
    static const u8 l_h_block_time[] = {12, 8, 8, 8};
    static const u8 l_b_block_time[] = {50, 6, 6, 6};

    m_ptn_no = (m_frame >> 3) & 0xFF;

    if (m_ptn_no >= 4) {
        m_ptn_no -= 4;
    }

    m_shapeAngle.y = m_frame << 10;
    m_frame++;
    
    if (m_frame >= 0x40) {
        m_frame = 0;
    }

    m_h_ptn_time -= 1;

    if (m_h_ptn_time == 0) {
        m_h_ptn_no += 1;
        if (m_h_ptn_no >= 4) {
            m_h_ptn_no = 0;
            m_h_shapeAngle.y = ((s16)m_h_shapeAngle.y + 0x3fff) & 0xc000;
        }

        m_h_ptn_time = l_h_block_time[m_h_ptn_no];
    }

    if (m_h_ptn_no != 0) {
        m_h_shapeAngle.y += 0x2aa;
    }

    m_b_ptn_time -= 1;
    if (m_b_ptn_time == 0) {
        m_b_ptn_no += 1;
        if (m_b_ptn_no >= 4) {
            m_b_ptn_no = 0;
        }

        m_b_ptn_time = l_b_block_time[m_b_ptn_no];
    }

    return 1;
}
