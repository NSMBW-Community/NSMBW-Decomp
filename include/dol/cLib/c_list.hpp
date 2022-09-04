#pragma once

// NOTE: Class and function names are unofficial!

class cListNd_c {
public:
    cListNd_c *mpPrev;
    cListNd_c *mpNext;
    void *mpOwner;
};

class cListMg_c {
public:
    cListNd_c *mpFirst;
    cListNd_c *mpLast;

    bool insertAfter(cListNd_c *pNode, cListNd_c *pPrevNode);
    bool remove(cListNd_c *pNode);
    bool append(cListNd_c *pNode);
    bool prepend(cListNd_c *pNode);
};
