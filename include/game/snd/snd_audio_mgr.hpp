#pragma once

class SndAudioMgr {
public:
    void startSystemSe(unsigned int soundID, unsigned long);
    void holdSystemSe(unsigned int soundID, unsigned long);

public:
    static SndAudioMgr *sInstance;
};
