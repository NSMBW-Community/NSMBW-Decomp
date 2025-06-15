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

    u32 GetId() const {
        return mId;
    }

    u8 mPad[0x9c];
    u32 mId;
};

class SeqSound {
public:
    void WriteVariable(int, short);
};

} // namespace detail

class SoundHandle {
public:
    SoundHandle() : mpSound(nullptr) {}
    ~SoundHandle() { DetachSound();}

    bool IsAttachedSound() const {
        return mpSound != nullptr;
    }

    void SetVolume(float f, int i) {
        if (IsAttachedSound()) {
            mpSound->SetVolume(f, i);
        }
    }
    void SetPan(float f) {
        if (IsAttachedSound()) {
            mpSound->SetPan(f);
        }
    }
    void SetPlayerPriority(int i) {
        if (IsAttachedSound()) {
            mpSound->SetPlayerPriority(i);
        }
    }

    u32 GetId() const {
        if (IsAttachedSound()) {
            return mpSound->GetId();
        }
        return -1;
    }

    void DetachSound();

    detail::BasicSound *mpSound;
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