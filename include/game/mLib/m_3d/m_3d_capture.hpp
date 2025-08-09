#pragma once
#include <lib/egg/core/eggHeap.h>
#include <revolution/GX.h>

namespace m3d {
    class capture_c {
    public:
        virtual ~capture_c() {}

        bool create(EGG::Heap *heap, u16 width, u16 height, GXTexFmt texFmt,
                    GXTexFilter texFilter, bool texIsHalfSize);

        void remove();

        void capture(u16 width, u16 height, bool texIsHalfSize);
        void captureEx(u16 width, u16 height, GXTexFmt texFmt, GXTexFmt tex2Fmt, GXTexFilter texFilter,
                       bool texIsHalfSize, u16 srcTexWidth, u16 srcTexHeight, bool srcTexHalfSize);

        GXTexObj mTexObj;
        void *mTexBuffer;
        size_t mBufferSize;

        u16 mTexWidth;
        u16 mTexHeight;
        GXTexFmt mTexFmt;
        GXTexFilter mTexFilter;
        bool mTexIsHalfSize;
    };
}
