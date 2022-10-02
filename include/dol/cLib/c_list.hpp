#pragma once

#include <types.h>

// [Unofficial name]
/**
 * @brief A doubly-linked list node.
 * @see cListMg_c
 */
class cListNd_c {
public:
    cListNd_c *mpPrev;
    cListNd_c *mpNext;
    
    cListNd_c() : mpPrev(nullptr), mpNext(nullptr) {}

    cListNd_c *getPrev() const { return mpPrev; }
    cListNd_c *getNext() const { return mpNext; }
};

// [Unofficial name]
/**
 * @brief A doubly-linked list container.
 * @see cListNd_c
 */
class cListMg_c {
public:
    cListNd_c *mpFirst;
    cListNd_c *mpLast;

    cListMg_c() : mpFirst(nullptr), mpLast(nullptr) {}

    bool insertAfter(cListNd_c *node, cListNd_c *prevNode);
    bool remove(cListNd_c *node);
    bool append(cListNd_c *node);
    bool prepend(cListNd_c *node);

    cListNd_c *getFirst() const { return mpFirst; }
    cListNd_c *getLast() const { return mpLast; }
};
