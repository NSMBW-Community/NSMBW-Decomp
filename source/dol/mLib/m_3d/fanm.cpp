#include <game/mLib/m_3d.hpp>

m3d::fanm_c::fanm_c() {
    mFrameMax = 0;
    mCurrFrame = 0;
    mPlayMode = FORWARD_LOOP;
}

m3d::fanm_c::~fanm_c() {}

void m3d::fanm_c::play() {
    float frame = mpObj->GetFrame();
    float updateRate = mpObj->GetUpdateRate();

    bool updateRateNegative = updateRate < 0.0f;
    if (updateRateNegative) {
        updateRate *= -1.0f;
    }

    mCurrFrame = frame;

    if (updateRateNegative || (mPlayMode & MASK_FORWARD) != 0) {
        // Reverse animation
        if (frame >= updateRate) {
            frame -= updateRate;
        } else {
            if ((mPlayMode & MASK_LOOP) == 0) {
                frame += mFrameMax - updateRate;
            } else {
                frame = mFrameStart;
            }
        }
    } else {
        // Forward animation
        frame += updateRate;
        if ((mPlayMode & MASK_LOOP) == 0) {
            if (frame >= mFrameMax) {
                frame -= mFrameMax;
            }
        } else {
            float endFrame = mFrameMax - 1.0f;
            if (frame >= endFrame) {
                frame = endFrame;
            }
        }
    }

    mpObj->SetFrame(frame);
}

void m3d::fanm_c::set(float duration, m3d::playMode_e playMode, float updateRate, float startFrame) {
    float frame = startFrame;
    if (startFrame < 0.0f) {
        if (playMode == FORWARD_ONCE) {
            frame = 0.0f;
        } else {
            frame = duration - 1.0f;
        }
    }

    mFrameMax = duration;
    mFrameStart = 0.0f;

    mpObj->SetFrame(frame);
    mpObj->SetUpdateRate(updateRate);

    mPlayMode = playMode;
    mCurrFrame = frame;
}

void m3d::fanm_c::setFrame(float f) {
    banm_c::setFrameOnly(f);
    mCurrFrame = f;
}

bool m3d::fanm_c::isStop() const {
    float frame = mpObj->GetFrame();
    float updateRate = mpObj->GetUpdateRate();

    // Only the "once" modes should ever be able to stop
    if (updateRate < 0.0f || mPlayMode == REVERSE_ONCE) {
        return frame <= mFrameStart;
    } else if (mPlayMode == FORWARD_ONCE) {
        return frame >= mFrameMax - 1.0f;
    }

    return false;
}

bool m3d::fanm_c::checkFrame(float f) const {
    float objFrame = mpObj->GetFrame();
    if (mCurrFrame == objFrame) {
        return objFrame == f;
    }

    float updateRate = mpObj->GetUpdateRate();

    if (updateRate < 0.0f || (mPlayMode & MASK_FORWARD) != 0) {
        // Reverse animation
        if (mCurrFrame > objFrame) {
            if (mCurrFrame > f && objFrame <= f) {
                return true;
            }
        } else {
            // mCurrFrame < objFrame
            if (f < mCurrFrame || f >= objFrame) {
                return true;
            }
        }
    } else {
        // Forward animation
        if (mCurrFrame < objFrame) {
            if (mCurrFrame < f && objFrame >= f) {
                return true;
            }
        } else {
            // mCurrFrame > objFrame
            if (f > mCurrFrame || f <= objFrame) {
                return true;
            }
        }
    }

    return false;
}
