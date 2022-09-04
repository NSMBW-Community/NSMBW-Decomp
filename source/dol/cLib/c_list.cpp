#include <dol/cLib/c_list.hpp>
#include <types.h>

// NOTE: Class and function names are unofficial!

bool cListMg_c::insertAfter(cListNd_c *pNode, cListNd_c *pPrevNode) {
    if (pPrevNode == nullptr)
        return prepend(pNode);

    if (pNode != nullptr) {
        pNode->mpNext = pPrevNode->mpNext;
        pNode->mpPrev = pPrevNode;
        pPrevNode->mpNext = pNode;

        if (pNode->mpNext != nullptr) {
            pNode->mpNext->mpPrev = pNode;
        } else {
            mpLast = pNode;
        }

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::remove(cListNd_c *pNode) {
    if (pNode != nullptr) {
        if (pNode->mpPrev != nullptr) {
            pNode->mpPrev->mpNext = pNode->mpNext;
        } else if (pNode == mpFirst) {
            mpFirst = pNode->mpNext;
        }

        if (pNode->mpNext != nullptr) {
            pNode->mpNext->mpPrev = pNode->mpPrev;
        } else if (pNode == mpLast) {
            mpLast = pNode->mpPrev;
        }

        pNode->mpPrev = nullptr;
        pNode->mpNext = nullptr;

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::append(cListNd_c *pNode) {
    if (pNode != nullptr) {
        if (mpLast != nullptr) {
            mpLast->mpNext = pNode;
            pNode->mpPrev = mpLast;
        } else {
            mpFirst = pNode;
        }
        mpLast = pNode;

    } else {
        return false;
    }

    return true;
}

bool cListMg_c::prepend(cListNd_c *pNode) {
    if (pNode != nullptr) {
        if (mpFirst != nullptr) {
            mpFirst->mpPrev = pNode;
            pNode->mpNext = mpFirst;
        } else {
            mpLast = pNode;
        }
        mpFirst = pNode;

    } else {
        return false;
    }

    return true;
}
