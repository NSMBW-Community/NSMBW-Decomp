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
    SoundHandle() : m_00(0), m_04(0.0f) {}
    ~SoundHandle() { nw4r::snd::SoundHandle::DetachSound();}

    void DetachSound();

    u32 m_00;
    float m_04;
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

    int GetPlayingSoundCount(int) const;
    void SetPlayableSoundCount(int, int);
};

} // namespace nw4r
} // namespace snd