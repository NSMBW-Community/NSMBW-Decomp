#include <dol/mLib/m_3d.hpp>

m3d::fanm_c::fanm_c() {
    mF1 = 0;
    mF3 = 0;
    mPlayMode = DEFAULT;
}

m3d::fanm_c::~fanm_c() {}

void m3d::fanm_c::play() {
    float frame = mpObj->GetFrame();
    float updateRate = mpObj->GetUpdateRate();

    bool updateRateNegative = updateRate < 0;

    if (updateRateNegative) {
        updateRate *= -1.0f;
    }

    mF3 = frame;

    if (updateRateNegative || (mPlayMode & 2) != 0) {
        if (frame >= updateRate) {
            frame -= updateRate;
        } else {
            if ((mPlayMode & 1) == 0) {
                frame += mF1 - updateRate;
            } else {
                frame = mF2;
            }
        }
    } else {
        frame += updateRate;
        if ((mPlayMode & 1) == 0) {
            if (frame >= mF1) {
                frame -= mF1;
            }
        } else {
            float updated = mF1 - 1.0f;
            if (frame >= updated) {
                frame = updated;
            }
        }
    }

    mpObj->SetFrame(frame);
}

void m3d::fanm_c::set(float f1, m3d::playMode_e pm, float f2, float f3) {
    float frame = f3;
    if (f3 < 0) {
        if (pm == REVERSE) {
            frame = 0;
        } else {
            frame = f1 - 1;
        }
    }

    mF1 = f1;
    mF2 = 0;

    mpObj->SetFrame(frame);
    mpObj->SetUpdateRate(f2);

    mPlayMode = pm;

    mF3 = frame;
}

void m3d::fanm_c::setFrame(float frame) {
    banm_c::setFrameOnly(frame);
    mF3 = frame;
}

bool m3d::fanm_c::isStop() const {
    float frame = mpObj->GetFrame();
    float updateRate = mpObj->GetUpdateRate();

    if (updateRate < 0 || mPlayMode == THREE) {
        return frame <= mF2;
    } else if (mPlayMode == REVERSE) {
        return frame >= (mF1 - 1);
    }

    return false;
}

bool m3d::fanm_c::checkFrame(float f) const {
    float frame = mpObj->GetFrame();
    if (mF3 == frame) {
        return frame == f;
    }
    
    float updateRate = mpObj->GetUpdateRate();

    if ((updateRate < 0) || ((mPlayMode & 2) != 0)) {
        updateRate = mF3;
        if (updateRate > frame) {
            if ((updateRate > f) && (frame <= f)) {
                return true;
            }
        } else if ((f < updateRate) || (f >= frame)) {
            return true;
        }
    } else {
        updateRate = mF3;
        if (updateRate < frame) {
            if ((updateRate < f) && (frame >= f)) {
                return true;
            }
        } else if ((f > updateRate) || (f <= frame)) {
            return true;
        }
    }

    return false;
}
