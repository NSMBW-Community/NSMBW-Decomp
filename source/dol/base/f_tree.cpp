#include <types.h>
#include <dol/base/f_tree_mg_ptmf.hpp>
#include <dol/base/f_tree_mg.hpp>
#include <dol/base/f_tree_nd.hpp>
#include <dol/base/f_base.hpp>

bool fTrMgPTMF_c::walkPack() {
    if (mpPack == 0) return true;
    
    fTrNdBa_c *curr = (fTrNdBa_c *) mpRootNode;
    while (curr != nullptr) {
        fTrNdBa_c *next = curr->getTreeNext();
        (curr->mpOwner->*mpPack)();
        curr = next;
    }
    return true;
}

const fTrNdBa_c *fTrMgBa_c::searchNodeByProfName(Profile prof, const fTrNdBa_c *parent) {
    const fTrNdBa_c *root;
    
    if (parent != nullptr) {
        root = parent->getTreeNext();
    } else {
        root = (fTrNdBa_c *) this->mpRootNode;
    }
    
    const fTrNdBa_c *curr = root;
    while (curr != nullptr) {
        if (curr->mpOwner->mProfName == prof) return curr;
        curr = curr->getTreeNext();
    }
    return nullptr;
}

const fTrNdBa_c *fTrMgBa_c::searchNodeByGroupType(u8 groupType, const fTrNdBa_c *parent) {
    const fTrNdBa_c *root;
    
    if (parent != nullptr) {
        root = parent->getTreeNext();
    } else {
        root = (fTrNdBa_c *) this->mpRootNode;
    }
    
    const fTrNdBa_c *curr = root;
    while (curr != nullptr) {
        if (curr->mpOwner->mGroupType == groupType) return curr;
        curr = curr->getTreeNext();
    }
    return nullptr;
}