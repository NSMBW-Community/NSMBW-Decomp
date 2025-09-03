#ifndef NW4R_SND_AXFX_IMPL_H
#define NW4R_SND_AXFX_IMPL_H
#include "nw4r/types_nw4r.h"
#include <revolution/AXFX.h> // IWYU pragma: export
#include <revolution/MEM.h>  // IWYU pragma: export

namespace nw4r {
namespace snd {
namespace detail {

struct AxfxImpl {
    MEMiHeapHead *mHeap; // at 0x4
    ulong mAllocCount;     // at 0x8

    static const ulong HEAP_SIZE_MIN = MEM_FRM_HEAP_MIN_SIZE + 32;

    AxfxImpl() : mHeap(NULL), mAllocCount(0) {}

    bool CreateHeap(void *pBuffer, ulong size);
    void DestroyHeap();

    ulong GetHeapTotalSize() {
        if (mHeap == NULL) {
            return 0;
        }

        return MEMGetHeapTotalSize(mHeap);
    }

    void HookAlloc(AXFXAllocHook *pAllocHook, AXFXFreeHook *pFreeHook);
    ulong RestoreAlloc(AXFXAllocHook allocHook, AXFXFreeHook freeHook);

    static void *Alloc(ulong size);
    static void Free(void *pBlock);

    static AxfxImpl *mCurrentFx;
    static ulong mAllocatedSize;
};

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
