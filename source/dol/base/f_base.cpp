#include <dol/base/f_base.hpp>
#include <dol/m/m_heap.hpp>
#include <lib/MSL_C/string.h>
#include <sjis_constants.h>

fBaseID_e fBase_c::m_rootUniqueID = FIRST_ID;
Profile fBase_c::m_tmpCtProfName;
u32 fBase_c::m_tmpCtParam;
u8 fBase_c::m_tmpCtGroupType;
fTrNdBa_c *fBase_c::m_tmpCtConnectParent;

void (*fBase_c::sLoadAsyncCallback)();
void (*fBase_c::sUnloadCallback)();

fBase_c::fBase_c() : 
    mUniqueID(m_rootUniqueID),
    mParam(m_tmpCtParam),
    mProfName(m_tmpCtProfName),
    mGroupType(m_tmpCtGroupType),
    mMng(this)
{
    m_rootUniqueID = (fBaseID_e) (m_rootUniqueID + 1);
    if (m_rootUniqueID == INVALID) {
        while (true);
    }

    fManager_c::m_connectManage.addTreeNode(&mMng.mConnectNode, m_tmpCtConnectParent);
    int idx = mMng.getSearchTableNum();
    fManager_c::m_searchManage[idx].prepend(&mMng.mSearchNode);

    // Try to get profile and set the order fields
    ProfileData *prof = (*g_profiles)[mProfName];
    if (prof != nullptr) {
        u16 executeOrder = prof->mExecuteOrder;
        mMng.mExecuteNode.mOrder = executeOrder;
        mMng.mExecuteNode.mNewOrder = executeOrder;
        u16 drawOrder = prof->mDrawOrder;
        mMng.mDrawNode.mOrder = drawOrder;
        mMng.mDrawNode.mNewOrder = drawOrder;
    }

    // Use process flags from parent
    fBase_c *parent = getConnectParent();
    if (parent != nullptr) {
        if (parent->isProcessFlag(fManager_c::PROC_FLAG_CONNECT) || parent->isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
            mProcessFlags |= fManager_c::PROC_FLAG_CREATE;
        }
        if (parent->isProcessFlag(fManager_c::PROC_FLAG_EXECUTE) || parent->isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
            mProcessFlags |= fManager_c::PROC_FLAG_DELETE;
        }
    }
}

fBase_c::~fBase_c() {
    fLiNdBa_c *curr = mUnusedList.getFirst();
    while (curr != nullptr) {
        curr->removeSelf();
        curr = mUnusedList.getFirst();
    }
}

int fBase_c::commonPack(int (fBase_c::*doFunc)(), int (fBase_c::*preFunc)(), void (fBase_c::*postFunc)(MAIN_STATE_e)) {
    MAIN_STATE_e arg;
    int result = (this->*preFunc)();
    if (result) {
        result = (this->*doFunc)();
        if (result == 0) {
            arg = WAITING;
        } else if (result == 1) {
            arg = SUCCESS;
        } else {
            arg = ERROR;
        }
    } else {
        arg = UNSUCCESSFUL;
    }
    (this->*postFunc)(arg);
    return result;
}

int fBase_c::create() {
    return 1;
}
int fBase_c::preCreate() {
    return 1;
}
void fBase_c::postCreate(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        fManager_c::m_createManage.remove(&mMng.mExecuteNode);
        if (fManager_c::m_nowLoopProc == fManager_c::EXECUTE) {
            // We cannot add the base to the execute and manage list during the execute process [(why?)],
            // so we delay it
            delayManageAdd = true;
        } else {
            fManager_c::m_executeManage.addNode(&mMng.mExecuteNode);
            fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
            mLifecycleState = ACTIVE;
        }
    } else if (state == ERROR) {
        // Something went wrong and we must discard the base.
        deleteRequest();
    }
}

int fBase_c::doDelete() {
    return 1;
}
int fBase_c::createPack() {
    return commonPack(&fBase_c::create, &fBase_c::preCreate, &fBase_c::postCreate);
}
int fBase_c::preDelete() {
    if (mpUnusedHelper != nullptr) {
        if (!mpUnusedHelper->LoadOnlyOne()) {
            return 0;
        }
    }

    // Assert base has no children
    fBase_c *child = getConnectChild();
    if (child != nullptr) {
        return 0;
    }
    return 1;
}
void fBase_c::postDelete(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        // Remove from all manager lists
        fManager_c::m_connectManage.removeTreeNode(&mMng.mConnectNode);
        fManager_c::m_searchManage[mMng.getSearchTableNum()].remove(&mMng.mSearchNode);
        fManager_c::m_deleteManage.remove(&mMng.mExecuteNode);

        if (mpHeap != nullptr) {
            mpHeap->destroy();
        }

        if (mpUnusedHelper != nullptr) {
            mpUnusedHelper->Delete();
        }

        delete this;
    }
}
int fBase_c::deletePack() {
    return commonPack(&fBase_c::doDelete, &fBase_c::preDelete, &fBase_c::postDelete);
}

int fBase_c::execute() {
    return 1;
}
int fBase_c::preExecute() {
    // Can only execute if not creating
    if (mDeleteRequested || isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
        return 0;
    }
    return 1;
}
void fBase_c::postExecute(MAIN_STATE_e state) {
    // Do nothing
    return;
}
int fBase_c::executePack() {
    return commonPack(&fBase_c::execute, &fBase_c::preExecute, &fBase_c::postExecute);
}

int fBase_c::draw() {
    return 1;
}
int fBase_c::preDraw() {
    // Can only execute if not deleting
    if (mDeleteRequested || isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
        return 0;
    }
    return 1;
}
void fBase_c::postDraw(MAIN_STATE_e state) {
    // Do nothing
    return;
}
int fBase_c::drawPack() {
    return commonPack(&fBase_c::draw, &fBase_c::preDraw, &fBase_c::postDraw);
}

void fBase_c::deleteReady() {
    // Do nothing
}

int fBase_c::connectProc() {
    if (mDeleteRequested) {
        mDeleteRequested = false;

        // Remove base from the relevant manager lists and add it to the delete list
        if (mLifecycleState == ACTIVE) {
            fManager_c::m_executeManage.remove(&mMng.mExecuteNode);
            fManager_c::m_drawManage.remove(&mMng.mDrawNode);
        } else {
            fManager_c::m_createManage.remove(&mMng.mExecuteNode);
        }
        fManager_c::m_deleteManage.prepend(&mMng.mExecuteNode);

        mLifecycleState = TO_BE_DELETED;

        // Also delete all children
        for (fTrNdBa_c *curr = mMng.mConnectNode.getChild(); curr != nullptr; curr = curr->getBrNext()) {
            curr->mpOwner->deleteRequest();
        }
    } else {
        // Copy over flags from parent
        fBase_c *parent = getConnectParent();
        if (parent != nullptr) {
            if (parent->isProcessFlag(fManager_c::PROC_FLAG_CONNECT) || parent->isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
                mProcessFlags |= fManager_c::PROC_FLAG_CREATE;
            } else if (isProcessFlag(fManager_c::PROC_FLAG_CREATE)) {
                mProcessFlags &= ~fManager_c::PROC_FLAG_CREATE;
            }
            
            if (parent->isProcessFlag(fManager_c::PROC_FLAG_EXECUTE) || parent->isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
                mProcessFlags |= fManager_c::PROC_FLAG_DELETE;
            } else if (isProcessFlag(fManager_c::PROC_FLAG_DELETE)) {
                mProcessFlags &= ~fManager_c::PROC_FLAG_DELETE;
            }
        }

        if (mLifecycleState == ACTIVE) {
            // Assert correct order in execute and draw list
            // [There's probably some inlining going on here, which is the reason
            // for all those duplicated temporaries. Couldn't figure out a better solution, though.]
            fLiNdPrio_c *executeNode = &mMng.mExecuteNode;
            if (executeNode->mNewOrder != executeNode->mOrder) {
                fManager_c::m_executeManage.remove(&mMng.mExecuteNode);
                fLiNdPrio_c *executeNode2 = &mMng.mExecuteNode;
                executeNode2->mOrder = executeNode2->mNewOrder;
                fManager_c::m_executeManage.addNode(executeNode2);
            }
            fLiNdPrio_c *drawNode = &mMng.mDrawNode;
            if (drawNode->mNewOrder != drawNode->mOrder) {
                fManager_c::m_drawManage.remove(&mMng.mDrawNode);
                fLiNdPrio_c *drawNode2 = &mMng.mDrawNode;
                drawNode2->mOrder = drawNode2->mNewOrder;
                fManager_c::m_drawManage.addNode(drawNode2);
            }
        } else if (mLifecycleState != TO_BE_DELETED) {
            // Means that the base has delayed adding itself to the manager lists, try again

            if (mRetryCreate) {
                mRetryCreate = false;
                fManager_c::m_createManage.append(&mMng.mExecuteNode);
            } else if (delayManageAdd) {
                delayManageAdd = false;
                fManager_c::m_executeManage.addNode(&mMng.mExecuteNode);
                fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
                mLifecycleState = ACTIVE;
            }
        }
    }
    return 1;
}

void fBase_c::deleteRequest() {
    // If not already trying to delete
    if (!mDeleteRequested && mLifecycleState != TO_BE_DELETED) {
        mDeleteRequested = true;
        deleteReady();
        
        // Also delete all children
        for (fTrNdBa_c *curr = mMng.mConnectNode.getChild(); curr != nullptr; curr = curr->getBrNext()) {
            curr->mpOwner->deleteRequest();
        }
    }
}

fBase_c *fBase_c::getConnectParent() {
    if (mMng.mConnectNode.getParent() != nullptr) {
        return mMng.mConnectNode.getParent()->mpOwner;
    }
    return nullptr;
}
fBase_c *fBase_c::getConnectChild() {
    if (mMng.mConnectNode.getChild() != nullptr) {
        return mMng.mConnectNode.getChild()->mpOwner;
    }
    return nullptr;
}
fBase_c *fBase_c::getConnectBrNext() {
    if (mMng.mConnectNode.getBrNext() != nullptr) {
        return mMng.mConnectNode.getBrNext()->mpOwner;
    }
    return nullptr;
}

bool fBase_c::entryFrmHeap(unsigned long size, EGG::Heap *parentHeap) {
    if (mpHeap != nullptr) {
        return true;
    }
    unsigned long heapSize = 0;
    EGG::FrmHeap *newHeap = nullptr;
    if (size != 0) {
        // First, try to make a heap with the given size
        newHeap = makeFrmHeapAndUpdate(size, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
        if (newHeap != nullptr) {
            bool createSuccess = createHeap();
            mHeap::restoreCurrentHeap();
            if (!createSuccess) {
                mHeap::destroyFrmHeap(newHeap);
                newHeap = nullptr;
            } else {
                heapSize = mHeap::adjustFrmHeap(newHeap);
                if (size == heapSize) {
                    mpHeap = newHeap;
                    return true;
                }
            }
        }
    }
    if (newHeap == nullptr) {
        // If that failed, try to make a heap with maximum size
        newHeap = makeFrmHeapAndUpdate(0xFFFFFFFF, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
        if (newHeap != nullptr) {
            bool createSuccess = createHeap();
            mHeap::restoreCurrentHeap();
            if (!createSuccess) {
                mHeap::destroyFrmHeap(newHeap);
                newHeap = nullptr;
            } else {
                heapSize = mHeap::adjustFrmHeap(newHeap);
            }
        }
    }
    if (newHeap != nullptr) {
        EGG::FrmHeap *largerHeap = makeFrmHeapAndUpdate(heapSize, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
        if (largerHeap != nullptr) {
            if (largerHeap < newHeap) {
                mHeap::destroyFrmHeap(newHeap);
                newHeap = nullptr;
                bool createSuccess = createHeap();
                mHeap::restoreCurrentHeap();
                if (!createSuccess) {
                    mHeap::destroyFrmHeap(largerHeap);
                } else {
                    mHeap::adjustFrmHeap(largerHeap);
                    mpHeap = largerHeap;
                    return true;
                }  
            } else {
                mHeap::restoreCurrentHeap();
                mHeap::destroyFrmHeap(largerHeap);
            }
        }
    }
    if (newHeap != nullptr) {
        mpHeap = newHeap;
        return true;
    }
    // Everything failed, delete self
    deleteRequest();
    return false;
}
bool fBase_c::entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap) {
    if (mpHeap != nullptr) {
        return true;
    }
    EGG::FrmHeap *newHeap = makeFrmHeapAndUpdate(size, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
    if (newHeap != nullptr) {
        bool createSuccess = createHeap();
        mHeap::restoreCurrentHeap();
        if (!createSuccess) {
            mHeap::destroyFrmHeap(newHeap);
        } else {
            mpHeap = newHeap;
            return true;
        }
    }
    // Everything failed, delete self
    deleteRequest();
    return false;
}

bool fBase_c::createHeap() {
    return true;
}

void *fBase_c::operator new(size_t size) {
    void *mem = EGG::Heap::alloc(size, 0xFFFFFFFC, mHeap::g_gameHeaps[0]);
    if (mem != nullptr) {
        memset(mem, 0, size);
    }
    return mem;
}

void fBase_c::operator delete(void *mem) {
    EGG::Heap::free(mem, mHeap::g_gameHeaps[0]);
}

void fBase_c::runCreate() {
    createPack();
    if (!mDeleteRequested && !delayManageAdd && mLifecycleState == WAITING_FOR_CREATE) {
        // Something went wrong while creating, try again
        if (fManager_c::m_nowLoopProc == fManager_c::CREATE) {
            mRetryCreate = true;
        } else {
            fManager_c::m_createManage.append(&mMng.mExecuteNode);
        }
    }
}

fBase_c *fBase_c::getNonReadyChild() const {
    const fTrNdBa_c *connectNode = &mMng.mConnectNode;
    
    fTrNdBa_c *end = connectNode->getTreeNextNotChild();
    fTrNdBa_c *curr = connectNode->getChild();
    while (curr != nullptr && curr != end) {
        if (curr->mpOwner->mLifecycleState == WAITING_FOR_CREATE) {
            return curr->mpOwner;
        }
        curr = curr->getTreeNext();
    }
    return nullptr;
}

bool fBase_c::hasNonReadyChild() const {
    return getNonReadyChild() != nullptr;
}

void fBase_c::setTmpCtData(Profile profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType) {
    m_tmpCtParam = param;
    m_tmpCtProfName = profName;
    m_tmpCtGroupType = groupType;
    m_tmpCtConnectParent = connectParent;
}

fBase_c *fBase_c::fBase_make(Profile profName, fTrNdBa_c *connectParent, unsigned long param, u8 groupType) {
    if ((*g_profiles)[profName] == nullptr) {
        return nullptr;
    }
    setTmpCtData(profName, connectParent, param, groupType);
    fBase_c *res = (fBase_c *) (*g_profiles)[profName]->mpClassInit();
    setTmpCtData(0, nullptr, 0, 0);
    if (res != nullptr) {
        res->runCreate();
    }
    return res;
}

fBase_c *fBase_c::createChild(Profile profName, fBase_c *connectParent, unsigned long param, u8 groupType) {
    if (connectParent == nullptr) {
        return nullptr;
    }
    return fBase_make(profName, &connectParent->mMng.mConnectNode, param, groupType);
}

fBase_c *fBase_c::createRoot(Profile profName, unsigned long param, u8 groupType) {
    return fBase_make(profName, nullptr, param, groupType);
}
