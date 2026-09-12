#include <game/cLib/c_counter.hpp>

u32 cCounter_c::m_gameFrame;
u32 cCounter_c::m_exeFrame;

void cCounter_c::clear() {
    m_gameFrame = 0;
    m_exeFrame = 0;
}
