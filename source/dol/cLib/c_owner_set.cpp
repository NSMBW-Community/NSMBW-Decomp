#include <dol/cLib/c_owner_set.hpp>

/// @brief [Looks like it might be a stripped assert?]
void unknown() {}

void cOwnerSetMg_c::add(cOwnerSetNd_c *nd, void *owner) {
    if (this == nullptr || nd == nullptr || owner == nullptr) {
        unknown();
    }
    if (nd != nullptr) {
        if (mpRoot == nullptr) {
            mpRoot = nd;
        } else {
            cOwnerSetNd_c *curr = mpRoot;
            while (curr->mpNext != nullptr) {
                if (curr->mpNext == nd) {
                    return;
                }
                curr = curr->mpNext;
            }
            // Node not yet in set, add it
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
    if (nd != nullptr && nd->mpOwner == owner) {
        if (mpRoot == nd) {
            mpRoot = nd->mpNext;
        } else {
            cOwnerSetNd_c *curr = mpRoot;
            while (curr->mpNext != nd) {
                if (!curr->mpNext)
                    return;
                curr = curr->mpNext;
            }
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
