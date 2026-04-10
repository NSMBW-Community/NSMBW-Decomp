#pragma once

#include <lib/egg/core/eggHeap.h>

/// @ingroup bases
namespace dHbm {
    class Manage_c {
    public:
        bool Load();
        void DrawMenu();
        void DrawIcon();
        void Calculate();

        static void CreateInstance(EGG::Heap *heap);
        static dHbm::Manage_c *GetInstance(); ///< Gets a pointer to the instance of this class.

        u8 mPad1[0x1d0];
        u32 m_1d0;
        u8 mPad2[0x4];
        u32 mFlags;
    };

} // namespace dHbm
