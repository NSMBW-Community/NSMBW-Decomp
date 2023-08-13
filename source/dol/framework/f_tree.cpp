#include <types.h>
#include <dol/framework/f_tree_mg_ptmf.hpp>
#include <dol/framework/f_tree_mg.hpp>
#include <dol/framework/f_tree_nd.hpp>
#include <dol/framework/f_base.hpp>

bool fTrMgPTMF_c::walkPack() {

    // Gracefully fail if the processing function isn't set
    if (mpProcFunc == 0) {
        return true;
    }

    // Call the processing function for each node in the tree
    fTrNdBa_c *curr = (fTrNdBa_c *) mpRootNode;
    while (curr != nullptr) {
        fTrNdBa_c *next = curr->getTreeNext();
        (curr->mpOwner->*mpProcFunc)();
        curr = next;
    }
    return true;
}

const fTrNdBa_c *fTrMgBa_c::searchNodeByProfName(ProfileName prof, const fTrNdBa_c *parent) const {
    const fTrNdBa_c *root;

    if (parent != nullptr) {
        root = parent->getTreeNext();
    } else {
        root = (fTrNdBa_c *) mpRootNode;
    }

    const fTrNdBa_c *curr = root;
    while (curr != nullptr) {
        if (curr->mpOwner->mProfName == prof) return curr;
        curr = curr->getTreeNext();
    }
    return nullptr;
}

const fTrNdBa_c *fTrMgBa_c::searchNodeByGroupType(u8 groupType, const fTrNdBa_c *parent) const {
    const fTrNdBa_c *root;

    if (parent != nullptr) {
        root = parent->getTreeNext();
    } else {
        root = (fTrNdBa_c *) mpRootNode;
    }

    const fTrNdBa_c *curr = root;
    while (curr != nullptr) {
        if (curr->mpOwner->mGroupType == groupType) return curr;
        curr = curr->getTreeNext();
    }
    return nullptr;
}
