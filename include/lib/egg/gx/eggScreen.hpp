#pragma once

#include <types.h>

namespace EGG {

class Screen {
public:
    char pad[0x100];

    static void Initialize(const unsigned short *, const unsigned short *, Screen *);
    static void SetTVModeDefault();
    void CopyFromAnother(const Screen &);
};

} // namespace EGG
