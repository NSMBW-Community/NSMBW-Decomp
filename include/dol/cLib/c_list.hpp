#pragma once

#include <types.h>

// Unofficial name
class cListNd_c {
public:
    cListNd_c *mpPrev;
    cListNd_c *mpNext;
    
    cListNd_c() : mpPrev(nullptr), mpNext(nullptr) {}

    inline cListNd_c *getPrev() { return mpPrev; }
    inline cListNd_c *getNext() { return mpNext; }
};

// Unofficial name
class cListMg_c {
public:
    cListNd_c *mpFirst;
    cListNd_c *mpLast;

    cListMg_c() : mpFirst(nullptr), mpLast(nullptr) {}

    inline cListNd_c *getFirst() { return mpFirst; }
    inline cListNd_c *getLast() { return mpLast; }

    bool insertAfter(cListNd_c *pNode, cListNd_c *pPrevNode);
    bool remove(cListNd_c *pNode);
    bool append(cListNd_c *pNode);
    bool prepend(cListNd_c *pNode);
};
