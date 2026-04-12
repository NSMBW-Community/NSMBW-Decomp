#pragma once

#include <lib/revolution/OS.h>

namespace EGG {

class Thread {
public:
    Thread(OSThread *, int);

    static void initialize();

    u8 mPad[0x4c];
};

} // namespace EGG
