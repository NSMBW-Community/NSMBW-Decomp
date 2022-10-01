#pragma once

#include <types.h>

typedef u16 Profile;

struct ProfileData {
    void *(*mpClassInit)();
    u16 mExecuteOrder;
    u16 mDrawOrder;
};

extern ProfileData *(*g_profiles)[750];