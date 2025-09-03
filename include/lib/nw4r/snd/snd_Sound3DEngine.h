#ifndef NW4R_SND_SOUND_3D_ENGINE_H
#define NW4R_SND_SOUND_3D_ENGINE_H

#include "nw4r/types_nw4r.h"
#include "nw4r/snd/snd_Sound3DCalculator.h"


namespace nw4r {
namespace snd {

class Sound3DManager;
class SoundAmbientParam;

class Sound3DEngine {
public:
    Sound3DEngine();
    virtual ~Sound3DEngine() {}

    virtual void UpdateAmbientParam(const Sound3DManager*, const Sound3DParam*, ulong, int, SoundAmbientParam*);
    virtual s32 GetAmbientPriority(const Sound3DManager*, const Sound3DParam*, ulong);
    virtual s32 GetRequiredVoiceOutCount(const Sound3DManager*, const Sound3DParam*, ulong);
    virtual void UpdateAmbientParam(const Sound3DManager*, const Sound3DParam*, ulong, ulong flags, SoundAmbientParam*);

private:
    Sound3DCalculator::CalcPanParam mPanParam; // at 0x04
};

} // namespace snd
} // namespace nw4r

#endif
