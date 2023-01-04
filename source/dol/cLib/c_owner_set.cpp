#include <dol/cLib/c_owner_set.hpp>

void unknown() {
}

void cOwnerSetMg_c::add(cOwnerSetNd_c *nd, void *owner) {
    if (this == nullptr || nd == nullptr || owner == nullptr) {
        unknown();
    }

    if (nd != nullptr) {
        // If the set is empty, set it as the root node
        if (mpRoot == nullptr) {
            mpRoot = nd;

        // Else check if it's in the set
        } else {
            cOwnerSetNd_c *curr = mpRoot;
            while (curr->mpNext != nullptr) {
                if (curr->mpNext == nd) {
                    return;
                }
                curr = curr->mpNext;
            }

            // If it isn't, add it
            curr->mpNext = nd;
        }

        nd->mpOwner = owner;
        nd->mpNext = nullptr;
    }
}

void cOwnerSetMg_c::remove(cOwnerSetNd_c *nd, void *owner) {
    if (this == nullptr || nd == nullptr || owner == nullptr) {
        unknown();
    }

    // Check that the owner matches
    if (nd != nullptr && nd->mpOwner == owner) {

        // If it's the root node, set the next node as the root
        if (mpRoot == nd) {
            mpRoot = nd->mpNext;

        // Else check if it's in the set
        } else {
            cOwnerSetNd_c *curr = mpRoot;
            while (curr->mpNext != nd) {
                if (!curr->mpNext)
                    return;
                curr = curr->mpNext;
            }

            // If it is, remove it
            curr->mpNext = nd->mpNext;
        }

        nd->mpOwner = nullptr;
        nd->mpNext = nullptr;
    }
}

void cOwnerSetMg_c::clear() {
    if (this == nullptr) {
        unknown();
    }

    cOwnerSetNd_c *curr = mpRoot;
    mpRoot = nullptr;
    while (curr != nullptr) {
        cOwnerSetNd_c *next = curr->mpNext;
        curr->mpOwner = nullptr;
        curr->mpNext = nullptr;
        curr = next;
    }
}
