#pragma once

class SndAudioMgr {
public:
    void startSystemSe(unsigned int, unsigned long);

public:
    static SndAudioMgr *sInstance;
};
