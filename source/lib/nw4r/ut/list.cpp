#include <lib/nw4r/ut/list.hpp>

namespace nw4r {
namespace ut {

#define GET_OBJ_NODE(list, obj) ((Link*)(((u32)(obj)) + (list)->mOffset))

void List_Init(List *list, u16 offset) {
    list->mpHead = nullptr;
    list->mpTail = nullptr;
    list->mCount = 0;
    list->mOffset = offset;
}

void List_Append(List *list, void *obj) {
    // If only one node is in the list, set it as both head and tail
    // Else place it at the end
    if (list->mpHead == nullptr) {
        Link *node = GET_OBJ_NODE(list, obj);
        node->mpNext = nullptr;
        node->mpPrev = nullptr;
        list->mpHead = obj;
        list->mpTail = obj;
        list->mCount++;

    } else {
        Link *node = GET_OBJ_NODE(list, obj);
        node->mpPrev = list->mpTail;
        node->mpNext = nullptr;

        // Link the previous node
        GET_OBJ_NODE(list, list->mpTail)->mpNext = obj;

        // Set the new tail
        list->mpTail = obj;
        list->mCount++;
    }
}

void List_Prepend(List *list, void *obj) {
    // If only one node is in the list, set it as both head and tail
    // Else place it at the start
    if (list->mpHead == nullptr) {
        Link *node = GET_OBJ_NODE(list, obj);
        node->mpNext = nullptr;
        node->mpPrev = nullptr;
        list->mpHead = obj;
        list->mpTail = obj;
        list->mCount++;

    } else {
        Link *node = GET_OBJ_NODE(list, obj);
        node->mpPrev = nullptr;
        node->mpNext = list->mpHead;

        // Link the following node
        GET_OBJ_NODE(list, list->mpHead)->mpPrev = obj;

        // Set the new head
        list->mpHead = obj;
        list->mCount++;
    }
}

void List_Insert(List *list, void *target, void *obj) {
    // If the target is null, insert the object at the end of the list
    // Else if the target is the list head, insert the object at the beginning
    // Else perform regular insertion
    if (target == nullptr) {
        List_Append(list, obj);

    } else if (target == list->mpHead) {
        List_Prepend(list, obj);

    } else {
        Link *node = GET_OBJ_NODE(list, obj);
        void *prevObj = GET_OBJ_NODE(list, target)->mpPrev;
        Link *prevNode = GET_OBJ_NODE(list, prevObj);

        // Update the node to be inserted
        node->mpPrev = prevObj;
        node->mpNext = target;

        // Update its neighbors
        prevNode->mpNext = obj;
        GET_OBJ_NODE(list, target)->mpPrev = obj;

        // Update the list
        list->mCount++;
    }
}

void List_Remove(List *list, void *obj) {
    Link *node = GET_OBJ_NODE(list, obj);

    // If the node is the list head, set the next node as the new head
    // Else punch a hole in the list
    if (node->mpPrev == nullptr) {
        list->mpHead = node->mpNext;
    } else {
        GET_OBJ_NODE(list, node->mpPrev)->mpNext = node->mpNext;
    }

    // If the node is the list tail, set the previous node as the new tail
    // Else punch a hole in the list
    if (node->mpNext == nullptr) {
        list->mpTail = node->mpPrev;
    } else {
        GET_OBJ_NODE(list, node->mpNext)->mpPrev = node->mpPrev;
    }

    // Reset the node
    node->mpPrev = nullptr;
    node->mpNext = nullptr;

    // Update the list
    list->mCount--;
}

void *List_GetNext(const List *list, const void *obj) {
    if (obj == nullptr) {
        return list->mpHead;
    }

    return GET_OBJ_NODE(list, obj)->mpNext;
}

void *List_GetPrev(const List *list, const void *obj) {
    if (obj == nullptr) {
        return list->mpTail;
    }

    return GET_OBJ_NODE(list, obj)->mpPrev;
}

} // namespace ut
} // namespace nw4r
