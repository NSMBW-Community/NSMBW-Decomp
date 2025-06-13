#pragma once
#include <types.h>

namespace nw4r {
namespace snd {

namespace detail {
class BasicSound {
public:
    struct AmbientInfo;

    void SetVolume(float, int);
    void SetPan(float);
    void SetPlayerPriority(int);

    u8 mPad[0x9c];
    u32 m_9c;
};

class SeqSound {
public:
    void WriteVariable(int, short);
};

} // namespace detail

class SoundHandle {
public:
    SoundHandle() : mpSound(nullptr), m_04(0.0f) {}
    ~SoundHandle() { nw4r::snd::SoundHandle::DetachSound();}

    void SetVolume(float f, int i) { if (mpSound != nullptr) mpSound->SetVolume(f, i); }
    void SetPan(float f) { if (mpSound != nullptr) mpSound->SetPan(f); }
    void SetPlayerPriority(int i) { if (mpSound != nullptr) mpSound->SetPlayerPriority(i); }

    u32 getID() { return (mpSound != nullptr) ? mpSound->m_9c : -1; }

    void DetachSound();

    detail::BasicSound *mpSound;
    float m_04;
};

class SoundStartable {
public:
    struct StartInfo;
    void detail_StartSound(SoundHandle *, unsigned long, const StartInfo *);
    void detail_HoldSound(SoundHandle *, unsigned long, const StartInfo *);
};

class SeqSoundHandle {
public:
    SeqSoundHandle(SoundHandle *handle);
    void DetachSound();

    detail::SeqSound *mpSeqSound;
};

class SoundArchive {
public:
    class SoundInfo {
    public:
        u8 mPad1[0xc];
        int m_c;
        u8 mPad2[0x10];
    };

    int GetSoundType(unsigned long) const;
    void ReadSoundInfo(unsigned long, SoundInfo *) const;
};

class SoundArchivePlayer;

class SoundActor : public SoundStartable {
public:
    SoundActor();
    virtual ~SoundActor() {}

    virtual void detail_SetupSound(SoundHandle *, unsigned long, bool, const SoundStartable::StartInfo *);
    virtual void detail_ConvertLabelStringToSoundId(const char *);
    virtual void SetupSound(SoundHandle *, unsigned long, const SoundStartable::StartInfo *, void *);
    virtual void detail_SetupSoundWithAmbientInfo(SoundHandle *, unsigned long, const SoundStartable::StartInfo *, detail::BasicSound::AmbientInfo *, void *);


    int GetPlayingSoundCount(int) const;
    void SetPlayableSoundCount(int, int);
};

} // namespace nw4r
} // namespace snd