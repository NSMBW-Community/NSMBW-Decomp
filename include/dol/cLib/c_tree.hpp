#pragma once

class cTreeNd_c {
public:
    cTreeNd_c *mpParent;
    cTreeNd_c *mpChild;
    cTreeNd_c *mpPrev;
    cTreeNd_c *mpNext;

    cTreeNd_c();
    void forcedClear();
    cTreeNd_c *getTreeNext() const;
    cTreeNd_c *getTreeNextNotChild() const;
};

class cTreeMg_c {
    cTreeNd_c *mpRootNode;

public:
    bool addTreeNode(cTreeNd_c *node, cTreeNd_c *parent);
    bool removeTreeNode(cTreeNd_c *node);
};
