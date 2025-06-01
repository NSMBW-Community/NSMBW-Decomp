#pragma once
#include <types.h>

namespace nw4r {
namespace snd {

namespace detail {
namespace BasicSound {
    struct AmbientInfo;
} // namespace detail
} // namespace snd

class SoundHandle {
public:
    SoundHandle() {}
    virtual ~SoundHandle() { nw4r::snd::SoundHandle::DetachSound();}

    void DetachSound();

    u8 mPad[4];
};

namespace SoundStartable {
    struct StartInfo;
} // namespace SoundStartable

class SoundArchivePlayer;

class SoundActor {
public:
    SoundActor();
    virtual ~SoundActor() {}

    virtual void detail_SetupSound(nw4r::snd::SoundHandle *, unsigned long, bool, const nw4r::snd::SoundStartable::StartInfo *);
    virtual void detail_ConvertLabelStringToSoundId(const char *);
    virtual void SetupSound(nw4r::snd::SoundHandle *, unsigned long, const nw4r::snd::SoundStartable::StartInfo *, void *);
    virtual void detail_SetupSoundWithAmbientInfo(nw4r::snd::SoundHandle *, unsigned long, const nw4r::snd::SoundStartable::StartInfo *, nw4r::snd::detail::BasicSound::AmbientInfo *, void *);

    void SetPlayableSoundCount(int, int);
};

} // namespace nw4r
} // namespace snd