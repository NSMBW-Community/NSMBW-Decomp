#pragma once
#include <types.h>
#include <lib/nw4r/snd.h>

class SndAudioMgr {
public:
    void startSystemSe(unsigned int soundID, unsigned long);
    u32 get3DCtrlFlag(unsigned long);
    void setSoundPosition(nw4r::snd::SoundHandle *p, const nw4r::math::VEC2 &pos);

    u8 mPad1[0x100];
    nw4r::snd::SoundArchive *mpSndArc;
    u8 mPad2[0x4b8];
    nw4r::snd::SoundArchivePlayer &mArcPlayer;

public:
    static SndAudioMgr *sInstance;
};
