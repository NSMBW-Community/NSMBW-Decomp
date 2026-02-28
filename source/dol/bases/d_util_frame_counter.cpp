#include <game/bases/d_util_frame_counter.hpp>

namespace Util {

FrameCounter_c::FrameCounter_c()
    : m_frame(0.0f),
      m_startFrame(0.0f),
      m_endFrame(0.0f),
      m_updateRate(0.0f),
      m_type(TYPE_ONETIME) {
    m_finished = true;
}

FrameCounter_c::~FrameCounter_c() {}

void FrameCounter_c::init(f32 startFrame, f32 endFrame) {
    m_startFrame = startFrame;
    m_endFrame = endFrame;

    m_frame = startFrame;
}

void FrameCounter_c::play(f32 updateRate, Type_e type, f32 startFrame, f32 endFrame) {
    m_startFrame = startFrame;
    m_endFrame = endFrame;

    play(updateRate, type);
}

void FrameCounter_c::play(f32 updateRate, Type_e type) {
    m_updateRate = updateRate;
    m_type = type;

    m_frame = m_startFrame;
}

void FrameCounter_c::calc() {
    m_frame += m_updateRate;
    m_finished = false;

    // Target frame is the end frame
    if (m_updateRate >= 0.0f && m_frame >= m_endFrame) {
        m_finished = true;

        switch (m_type) {
            case TYPE_ONETIME:
                m_frame = m_endFrame;
                break;
            case TYPE_LOOP:
                m_frame = m_startFrame + (m_frame - m_endFrame);
                break;
            case TYPE_OSCILLATING:
                m_frame = m_endFrame - (m_frame - m_endFrame);
                m_updateRate *= -1.0f;
                break;
            default:
                break;
        }
    }

    // Target frame is the start frame
    if (m_updateRate <= 0.0f && m_frame <= m_startFrame) {
        m_finished = true;

        switch (m_type) {
            case TYPE_ONETIME:
                m_frame = m_startFrame;
                break;
            case TYPE_LOOP:
                m_frame = m_endFrame + (m_frame - m_startFrame);
                break;
            case TYPE_OSCILLATING:
                m_frame = m_startFrame - (m_frame - m_startFrame);
                m_updateRate *= -1.0f;
                break;
            default:
                break;
        }
    }
}

} // namespace Util
