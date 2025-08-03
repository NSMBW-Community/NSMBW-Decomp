#pragma once
#include <types.h>
///@file

namespace nw4hbm {
namespace ut {

/// @brief See nw4r::ut::Link.
struct Link {
    void *mpPrev;
    void *mpNext;
};

/// @brief See nw4r::ut::List.
struct List {
    void *mpHead;
    void *mpTail;
    u16 mCount;
    u16 mOffset;
};

void List_Init(List *list, u16 offset); ///< See include/lib/nw4r/ut/list.h.
void List_Append(List *list, void *obj); ///< See include/lib/nw4r/ut/list.h.
void List_Remove(List *list, void *obj); ///< See include/lib/nw4r/ut/list.h.
void *List_GetNext(const List *list, const void *obj); ///< See include/lib/nw4r/ut/list.h.

/// @brief Gets the nth linked object, starting from the top of the list.
/// @details Returns @p nullptr if the index is not valid.
void *List_GetNth(const List *list, u16 n);

} // namespace ut
} // namespace nw4hbm
