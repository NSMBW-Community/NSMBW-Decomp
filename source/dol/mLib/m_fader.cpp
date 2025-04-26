#include <game/mLib/m_3d.hpp>
#include <lib/nw4r/g3d/state.hpp>
#include <game/mLib/m_fader.hpp>
#include <game/mLib/m_fader_base.hpp>

mFaderBase_c *mFader_c::mFader = nullptr;

void mFader_c::draw() {
    nw4r::g3d::G3DState::Invalidate(nw4r::g3d::G3DState::INVALIDATE_ALL);
    m3d::resetMaterial();
    mFader->draw();
}

bool mFader_c::setFader(mFaderBase_c *fader) {

    // Make sure no other fader is currently in the process of fading the screen
    if (mFader != nullptr) {
        bool cannotSet = (isStatus(mFaderBase_c::FADE_IN) || isStatus(mFaderBase_c::FADE_OUT));
        if (cannotSet) {
            return false;
        }
    }

    // Get the fader's status
    mFaderBase_c::EStatus status;
    if (mFader != nullptr) {
        status = mFader->getStatus();
    } else {
        status = fader->getStatus();
    }

    // Set the fader and its status
    mFader = fader;
    switch(status) {
        case mFaderBase_c::OPAQUE: mFader->setStatus(mFaderBase_c::OPAQUE); break;
        case mFaderBase_c::HIDDEN: mFader->setStatus(mFaderBase_c::HIDDEN); break;
    }

    return true;
}
