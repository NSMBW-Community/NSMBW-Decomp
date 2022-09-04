#include <dol/cLib/c_tree.hpp>
#include <types.h>

cTreeNd_c::cTreeNd_c() {
    forcedClear();
}

void cTreeNd_c::forcedClear() {
    mpParent = nullptr;
    mpChild = nullptr;
    mpPrev = nullptr;
    mpNext = nullptr;
}

bool cTreeMg_c::addTreeNode(cTreeNd_c* node, cTreeNd_c* parent) {
    if (node != nullptr) {
        if (parent != nullptr) {
            node->mpParent = parent;
            cTreeNd_c* currChild = parent->mpChild;

            if (currChild == nullptr) {
                parent->mpChild = node;
            } else {
                while (currChild->mpNext != nullptr) {
                    currChild = currChild->mpNext;
                }
                currChild->mpNext = node;
                node->mpPrev = currChild;
            }

        } else {
            cTreeNd_c* currNode = mpRootNode;

            if (currNode != nullptr) {
                while (currNode->mpNext != nullptr) {
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
    if (node != nullptr) {
        if (node->mpChild != nullptr) {
            return false;
        }

        if (node->mpPrev != nullptr) {
            node->mpPrev->mpNext = node->mpNext;
        } else if (node->mpParent != nullptr) {
            node->mpParent->mpChild = node->mpNext;
        } else if (node == this->mpRootNode) {
            this->mpRootNode = node->mpNext;
        }

        if (node->mpNext != nullptr) {
            node->mpNext->mpPrev = node->mpPrev;
        }

        node->mpPrev = nullptr;
        node->mpNext = nullptr;
        node->mpParent = nullptr;

    } else {
        return false;
    }

    return true;
}

cTreeNd_c * cTreeNd_c::getTreeNext() const {
    cTreeNd_c* child = this->mpChild;

    if (child != nullptr) {
        return child;
    }

    return this->getTreeNextNotChild();
}

cTreeNd_c* cTreeNd_c::getTreeNextNotChild() const {
    if (this->mpNext != nullptr) {
        return this->mpNext;
    }

    cTreeNd_c* currParent = this->mpParent;
    while (currParent != nullptr) {
        if (currParent->mpNext != nullptr) {
            return currParent->mpNext;
        }
        currParent = currParent->mpParent;
    }

    return nullptr;
}
