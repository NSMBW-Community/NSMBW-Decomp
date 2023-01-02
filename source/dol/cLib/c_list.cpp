#include <dol/cLib/c_list.hpp>
#include <types.h>

bool cListMg_c::insertAfter(cListNd_c *node, cListNd_c *prevNode) {

    // If no previous node is given, prepend the new node
    if (prevNode == nullptr)
        return prepend(node);

    if (node != nullptr) {
        node->mpNext = prevNode->mpNext;
        node->mpPrev = prevNode;
        prevNode->mpNext = node;

        // If the node has a successor, link it as well
        // Else set the new node as last
        if (node->mpNext != nullptr) {
            node->mpNext->mpPrev = node;
        } else {
            mpLast = node;
        }

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::remove(cListNd_c *node) {
    if (node != nullptr) {

        // If the node has a predecessor, update it
        // Else set the next node as the first node
        if (node->mpPrev != nullptr) {
            node->mpPrev->mpNext = node->mpNext;
        } else if (node == mpFirst) {
            mpFirst = node->mpNext;
        }

        // If the node has a successor, update it
        // Else set the previous node as the last node
        if (node->mpNext != nullptr) {
            node->mpNext->mpPrev = node->mpPrev;
        } else if (node == mpLast) {
            mpLast = node->mpPrev;
        }

        // Clean the fields
        node->mpPrev = nullptr;
        node->mpNext = nullptr;

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::append(cListNd_c *node) {
    if (node != nullptr) {

        // If the last node exists, update it
        // Else set the new node as the first one as well
        if (mpLast != nullptr) {
            mpLast->mpNext = node;
            node->mpPrev = mpLast;
        } else {
            mpFirst = node;
        }
        mpLast = node;

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::prepend(cListNd_c *node) {
    if (node != nullptr) {

        // If the first node exists, update it
        // Else set the new node as the last one as well
        if (mpFirst != nullptr) {
            mpFirst->mpPrev = node;
            node->mpNext = mpFirst;
        } else {
            mpLast = node;
        }
        mpFirst = node;

    } else {
        return false;
    }

    return true;
}
