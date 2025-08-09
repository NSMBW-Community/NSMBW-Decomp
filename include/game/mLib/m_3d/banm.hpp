#pragma once
#include <game/mLib/m_allocator.hpp>
#include <lib/egg/core/eggFrmHeap.h>
#include <nw4r/g3d.h>

namespace m3d {
    enum playMode_e {
        FORWARD_LOOP, ///< Play the animation forward in a loop.
        FORWARD_ONCE, ///< Play the animation forward once.
        REVERSE_LOOP, ///< Play the animation in reverse in a loop.
        REVERSE_ONCE, ///< Play the animation in reverse once.
        PLAYMODE_INHERIT, ///< Use the play mode of the parent.

        MASK_LOOP = 1, ///< Mask for loop play mode.
        MASK_FORWARD = 2, ///< Mask for forward play mode.
    };

    class banm_c {
    public:
        /// @unofficial
        enum anmType_e {
            TYPE_ANM_CHR,
            TYPE_ANM_VIS,
            TYPE_ANM_MAT_CLR,
            TYPE_ANM_TEX_PAT,
            TYPE_ANM_TEX_SRT,
            TYPE_ANM_OTHER ///< Never used
        };

        banm_c() : mpObj(nullptr), mpHeap(nullptr) {}
        virtual ~banm_c();
        virtual anmType_e getType() const = 0;
        virtual void remove();
        virtual void play() {}

        bool createAllocator(mAllocator_c *allocator, size_t *size);
        bool IsBound() const;
        float getFrame() const;
        void setFrameOnly(float frame);
        float getRate() const;
        void setRate(float rate);

        nw4r::g3d::AnmObj *getObj() { return mpObj; }

    protected:
        nw4r::g3d::AnmObj *mpObj;
        EGG::FrmHeap *mpHeap;
        mAllocator_c mAllocator;

    private:
        static u32 sHeapAlignment; ///< @unofficial
    };
}
