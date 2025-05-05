#pragma once

#include <types.h>

namespace nw4r {
namespace snd {

// TODO
namespace detail {
    class BasicSound {
        public:
        class AmbientInfo {
        };
    };
} // namespace detail

// TODO
class SoundStartable {
    public:
    class StartInfo {
    };
};

class SoundActorParam {
    float mVolume;
    float mPitch;
    float mPan;
};

typedef void * SoundHandle; // TODO: figure out what this points to

class SoundActor {
    virtual ~SoundActor();
    virtual u32 detail_SetupSound(SoundHandle, unsigned long, bool, const SoundStartable::StartInfo *);
    virtual u32 detail_ConvertLabelStringToSoundId(const char *);
    virtual u32 SetupSound(SoundHandle, unsigned long, const SoundStartable::StartInfo *, void *);
    virtual u32 detail_SetupSoundWithAmbientInfo(SoundHandle, unsigned long, const SoundStartable::StartInfo *, detail::BasicSound::AmbientInfo *, void *);

    const int GetPlayingSoundCount(int);
    void SetPlayableSoundCount(int, int);

    void * mSoundArchivePlayer; // TODO: nw4r::snd::SoundArchivePlayer
    u8 mActorPlayer[0x10][4]; // TODO: nw4r::snd::ExternalSoundPlayer
    SoundActorParam mParam;
};

} // namespace snd
} // namespace nw4r
