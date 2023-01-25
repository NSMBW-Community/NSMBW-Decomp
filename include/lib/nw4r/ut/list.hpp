#pragma once
#include <types.h>
///@file

namespace nw4r {
namespace ut {

/// @brief A doubly-linked list node. See List.
struct Link {
    void *mpPrev; ///< The previous linked object.
    void *mpNext; ///< The next linked object.
};

/// @brief A doubly-linked list container. See Link.
struct List {
    void *mpHead; ///< The first linked object.
    void *mpTail; ///< The last linked object.
    u16 mCount; ///< The linked object count.
    u16 mOffset; ///< The offset of the Link structure in each object.
};

void List_Init(List *list, u16 offset); ///< Initializes the list.
void List_Append(List *list, void *obj); ///< Adds an object to the end of the list.
void List_Prepend(List *list, void *obj); ///< Adds an object to the beginning of the list.

/// @brief Inserts an object at a specified position in the list.
/// @details If @p target is not @p nullptr , @p obj is inserted in front of @p target ,
/// else it is added at the end of the list.
void List_Insert(List *list, void *target, void *obj);

void List_Remove(List *list, void *obj); ///< Deletes an object from the list.

/// @brief Gets the object linked after the given one.
/// @details If the @p obj is @p nullptr , the list's head object is returned.
void *List_GetNext(const List *list, const void *obj);

/// @brief Gets the object linked before the given one.
/// @details If the @p obj is @p nullptr , the list's tail object is returned.
void *List_GetPrev(const List *list, const void *obj);

} // namespace ut
} // namespace nw4r
