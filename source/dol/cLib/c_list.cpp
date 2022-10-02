#include <dol/cLib/c_list.hpp>
#include <types.h>

bool cListMg_c::insertAfter(cListNd_c *node, cListNd_c *prevNode) {
    if (prevNode == nullptr)
        return prepend(node);

    if (node != nullptr) {
        node->mpNext = prevNode->mpNext;
        node->mpPrev = prevNode;
        prevNode->mpNext = node;

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
        if (node->mpPrev != nullptr) {
            node->mpPrev->mpNext = node->mpNext;
        } else if (node == mpFirst) {
            mpFirst = node->mpNext;
        }

        if (node->mpNext != nullptr) {
            node->mpNext->mpPrev = node->mpPrev;
        } else if (node == mpLast) {
            mpLast = node->mpPrev;
        }

        node->mpPrev = nullptr;
        node->mpNext = nullptr;

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::append(cListNd_c *node) {
    if (node != nullptr) {
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
