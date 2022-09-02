#include "dol/cLib/c_tree.h"
#include "types.h"

cTreeNd_c::cTreeNd_c() {
    forcedClear();
}

void cTreeNd_c::forcedClear() {
    mpParent = NULL;
    mpChild = NULL;
    mpPrev = NULL;
    mpNext = NULL;
}

bool cTreeMg_c::addTreeNode(cTreeNd_c* node, cTreeNd_c* parent) {
    if (node != NULL) {
        if (parent != NULL) {
            node->mpParent = parent;
            cTreeNd_c* currChild = parent->mpChild;
            if (currChild == NULL) {
                parent->mpChild = node;
            } else {
                while (currChild->mpNext != NULL) {
                    currChild = currChild->mpNext;
                }
                currChild->mpNext = node;
                node->mpPrev = currChild;
            }
        } else {
            cTreeNd_c* currNode = mpRootNode;
            if (currNode != NULL) {
                while (currNode->mpNext != NULL) {
                    currNode = currNode->mpNext;
                }
                currNode->mpNext = node;
                node->mpPrev = currNode;
            } else {
                mpRootNode = node;
            }
        }
    } else {
        return false;
    }

    return true;
}

bool cTreeMg_c::removeTreeNode(cTreeNd_c* node) {
    if (node != NULL) {
        if (node->mpChild != NULL) {
            return false;
        }

        if (node->mpPrev != NULL) {
            node->mpPrev->mpNext = node->mpNext;
        } else if (node->mpParent != NULL) {
            node->mpParent->mpChild = node->mpNext;
        } else if (node == this->mpRootNode) {
            this->mpRootNode = node->mpNext;
        }

        if (node->mpNext != NULL) {
            node->mpNext->mpPrev = node->mpPrev;
        }

        node->mpPrev = NULL;
        node->mpNext = NULL;
        node->mpParent = NULL;
    } else {
        return false;
    }

    return true;
}

cTreeNd_c * cTreeNd_c::getTreeNext() {
    cTreeNd_c* child = this->mpChild;

    if (child != NULL) {
        return child;
    }

    return this->getTreeNextNotChild();
}

cTreeNd_c* cTreeNd_c::getTreeNextNotChild() {
    if (this->mpNext != NULL) {
        return this->mpNext;
    }

    cTreeNd_c* currParent = this->mpParent;
    while (currParent != NULL) {
        if (currParent->mpNext != NULL) {
            return currParent->mpNext;
        }
        currParent = currParent->mpParent;
    }

    return NULL;
}