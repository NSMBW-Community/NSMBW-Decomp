#include <game/bases/d_coin.hpp>
#include <types.h>

int dCoin_c::execute() {
    static u8 l_h_block_time[] = {12, 8, 8, 8};
    static u8 l_b_block_time[] = {50, 6, 6, 6};

    m_ptn_no = (m_frame >> 3) & 0xFF;

    if (m_ptn_no >= 4) {
        m_ptn_no -= 4;
    }

    m_shapeAngle[1] = (m_ptn_no << 10) & 0x3FC00;
    m_frame += 1;
    
    if ((u8)m_shapeAngle[1] >= 0x40) {
        m_frame = 0;
    }

    m_h_ptn_time -= 1;

    if (m_h_ptn_time == 0) {
        m_h_ptn_no += 1;
        if (m_h_ptn_no >= 4) {
            m_h_ptn_no = 0;
            m_h_shapeAngle[1] = m_h_shapeAngle[1] + 0x3fff & 0xc000;
        }

        m_h_ptn_time = l_h_block_time[m_h_ptn_no];
    }

    if (m_h_ptn_no != 0) {
        m_h_shapeAngle[1] = m_h_shapeAngle[1] + 0x2aa;
    }

    m_b_ptn_time = m_b_ptn_time + -1;
    if (m_b_ptn_time == 0) {
        m_b_ptn_no = m_b_ptn_no + 1;
        if (m_b_ptn_no >= 4) {
            m_b_ptn_no = 0;
        }

        m_b_ptn_time = l_b_block_time[m_b_ptn_no];
    }
    
    return 1;
}