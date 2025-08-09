#include <nw4r/ut.h>

namespace nw4r {
namespace ut {

void List_Init(List *list, u16 offset) {
    list->headObject = nullptr;
    list->tailObject = nullptr;
    list->numObjects = 0;
    list->offset = offset;
}

void List_Append(List *list, void *obj) {
    // If only one node is in the list, set it as both head and tail
    // Else place it at the end
    if (list->headObject == nullptr) {
        Link *node = NW4R_UT_LIST_GET_LINK(*list, obj);
        node->nextObject = nullptr;
        node->prevObject = nullptr;
        list->headObject = obj;
        list->tailObject = obj;
        list->numObjects++;

    } else {
        Link *node = NW4R_UT_LIST_GET_LINK(*list, obj);
        node->prevObject = list->tailObject;
        node->nextObject = nullptr;

        // Link the previous node
        NW4R_UT_LIST_GET_LINK(*list, list->tailObject)->nextObject = obj;

        // Set the new tail
        list->tailObject = obj;
        list->numObjects++;
    }
}

void List_Prepend(List *list, void *obj) {
    // If only one node is in the list, set it as both head and tail
    // Else place it at the start
    if (list->headObject == nullptr) {
        Link *node = NW4R_UT_LIST_GET_LINK(*list, obj);
        node->nextObject = nullptr;
        node->prevObject = nullptr;
        list->headObject = obj;
        list->tailObject = obj;
        list->numObjects++;

    } else {
        Link *node = NW4R_UT_LIST_GET_LINK(*list, obj);
        node->prevObject = nullptr;
        node->nextObject = list->headObject;

        // Link the following node
        NW4R_UT_LIST_GET_LINK(*list, list->headObject)->prevObject = obj;

        // Set the new head
        list->headObject = obj;
        list->numObjects++;
    }
}

void List_Insert(List *list, void *target, void *obj) {
    // If the target is null, insert the object at the end of the list
    // Else if the target is the list head, insert the object at the beginning
    // Else perform regular insertion
    if (target == nullptr) {
        List_Append(list, obj);

    } else if (target == list->headObject) {
        List_Prepend(list, obj);

    } else {
        Link *node = NW4R_UT_LIST_GET_LINK(*list, obj);
        void *prevObj = NW4R_UT_LIST_GET_LINK(*list, target)->prevObject;
        Link *prevNode = NW4R_UT_LIST_GET_LINK(*list, prevObj);

        // Update the node to be inserted
        node->prevObject = prevObj;
        node->nextObject = target;

        // Update its neighbors
        prevNode->nextObject = obj;
        NW4R_UT_LIST_GET_LINK(*list, target)->prevObject = obj;

        // Update the list
        list->numObjects++;
    }
}

void List_Remove(List *list, void *obj) {
    Link *node = NW4R_UT_LIST_GET_LINK(*list, obj);

    // If the node is the list head, set the next node as the new head
    // Else punch a hole in the list
    if (node->prevObject == nullptr) {
        list->headObject = node->nextObject;
    } else {
        NW4R_UT_LIST_GET_LINK(*list, node->prevObject)->nextObject = node->nextObject;
    }

    // If the node is the list tail, set the previous node as the new tail
    // Else punch a hole in the list
    if (node->nextObject == nullptr) {
        list->tailObject = node->prevObject;
    } else {
        NW4R_UT_LIST_GET_LINK(*list, node->nextObject)->prevObject = node->prevObject;
    }

    // Reset the node
    node->prevObject = nullptr;
    node->nextObject = nullptr;

    // Update the list
    list->numObjects--;
}

void *List_GetNext(const List *list, const void *obj) {
    if (obj == nullptr) {
        return list->headObject;
    }

    return NW4R_UT_LIST_GET_LINK(*list, obj)->nextObject;
}

void *List_GetPrev(const List *list, const void *obj) {
    if (obj == nullptr) {
        return list->tailObject;
    }

    return NW4R_UT_LIST_GET_LINK(*list, obj)->prevObject;
}

} // namespace ut
} // namespace nw4r
