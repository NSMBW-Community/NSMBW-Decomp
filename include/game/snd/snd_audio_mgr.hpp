#pragma once
#include <types.h>
#include <lib/nw4r/snd/snd_actor.hpp>

class SndAudioMgr {
public:
    void startSystemSe(unsigned int soundID, unsigned long);
    u32 get3DCtrlFlag(unsigned long);

    u8 mPad[0x5bc];
    nw4r::snd::SoundArchivePlayer &mArcPlayer;

public:
    static SndAudioMgr *sInstance;
};
