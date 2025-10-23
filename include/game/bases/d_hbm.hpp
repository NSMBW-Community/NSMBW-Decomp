#pragma once
#include <types.h>

/// @ingroup bases
namespace dHbm {
    class Manage_c {
    public:
        bool Load();

        u8 mPad[0x1d8];
        u32 mFlags;

        static dHbm::Manage_c* GetInstance(); ///< Gets a pointer to the instance of this class.
    };

} // namespace dHbm
