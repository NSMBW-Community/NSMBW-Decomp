#pragma once

// [Unofficial name]
/// @brief [A helper class for fBase_c with unknown purpose.]
class fBaHelper_c {
    short mStatus;
    short mCount;
    void **mpArr;
    void *filler1;
    short filler2;
    short mUnknown;

public:
    virtual void vf_0x8(); ///< [Stripped out of binary.]
    virtual int vf_0xc(void *); ///< [Stripped out of binary.]
    virtual void vf_0x10(void *); ///< [Stripped out of binary.]

    void Delete(); ///< [TODO: document this method]
    int Load(int); ///< [TODO: document this method]
    bool LoadOnlyOne(); ///< [TODO: document this method]
};