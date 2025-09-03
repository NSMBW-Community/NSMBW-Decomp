#ifndef NW4R_SND_SOUND_3D_LISTENER_H
#define NW4R_SND_SOUND_3D_LISTENER_H
#include <types.h>
#include "nw4r/types_nw4r.h"
#include "nw4r/ut/ut_LinkList.h"

#include "nw4r/math.h"

namespace nw4r {
namespace snd {

class Sound3DListener {
public:
    Sound3DListener();

    const math::MTX34 &GetMatrix() const {
        return mMtx;
    }

    const math::VEC3 &GetPosition() const {
        return mPosition;
    }

    const math::VEC3 &GetVelocity() const {
        return mVelocity;
    }

    void SetMatrix(const math::MTX34 &mtx);

    f32 GetInteriorSize() const {
        return mInteriorSize;
    }
    void SetInteriorSize(f32 size);

    f32 GetMaxVolumeDistance() const {
        return mMaxVolumeDistance;
    }
    void SetMaxVolumeDistance(f32 distance);

    f32 GetUnitDistance() const {
        return mUnitDistance;
    }
    void SetUnitDistance(f32 distance);

    f32 GetUnitBiquadFilterValue() const {
        return mUnitBiquadFilterValue;
    }
    void SetUnitBiquadFilterValue(f32 value);

    f32 GetUnitBiquadFilterMax() const {
        return mUnitBiquadFilterMax;
    }
    void SetUnitBiquadFilterMax(f32 value);

private:
    math::MTX34 mMtx;           // at 0x0
    math::VEC3 mPosition;       // at 0x30
    math::VEC3 mVelocity;       // at 0x3C
    f32 mInteriorSize;          // at 0x48
    f32 mMaxVolumeDistance;     // at 0x4C
    f32 mUnitDistance;          // at 0x50
    UNKWORD field_0x54;         // at 0x54
    u8 mSkipVelocityUpdate;     // at 0x58
    f32 mUnitBiquadFilterValue; // at 0x5C
    f32 mUnitBiquadFilterMax;   // at 0x60
    ut::LinkListNode mNode;     // at 0x64
};

} // namespace snd
} // namespace nw4r

#endif
