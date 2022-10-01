#include <dol/base/f_base.hpp>
#include <dol/m/m_heap.hpp>
#include <lib/MSL_C/string.h>

fBaseID_e fBase_c::m_rootUniqueID = (fBaseID_e) 1;
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

    ProfileData *prof = (*g_profiles)[mProfName];
    if (prof != nullptr) {
        u16 executeOrder = prof->mExecuteOrder;
        mMng.mExecuteNode.mOrder = executeOrder;
        mMng.mExecuteNode.mOrder2 = executeOrder;
        u16 drawOrder = prof->mDrawOrder;
        mMng.mDrawNode.mOrder = drawOrder;
        mMng.mDrawNode.mOrder2 = drawOrder;
    }
    fBase_c *parent = getConnectParent();
    if (parent != nullptr) {
        u8 procFlags = parent->mProcessFlags;
        if ((procFlags & 1) != 0 || (procFlags & 2) != 0) {
            mProcessFlags |= 2;
        }
        procFlags = parent->mProcessFlags;
        if ((procFlags & 4) != 0 || (procFlags & 8) != 0) {
            mProcessFlags |= 8;
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

int fBase_c::commonPack(int (fBase_c::*doFunc)(), bool (fBase_c::*preFunc)(), void (fBase_c::*postFunc)(MAIN_STATE_e)) {
    MAIN_STATE_e arg;
    int success = (this->*preFunc)();
    if (success) {
        // TODO: give better names, make doFunc return an enum
        success = (this->*doFunc)();
        if (success == 0) {
            arg = WAITING;
        } else if (success == 1) {
            arg = SUCCESS;
        } else {
            arg = ERROR;
        }
    } else {
        arg = UNSUCCESSFUL;
    }
    (this->*postFunc)(arg);
    return success;
}

int fBase_c::create() {
    return 1;
}
bool fBase_c::preCreate() {
    return true;
}
void fBase_c::postCreate(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        fManager_c::m_createManage.remove(&mMng.mExecuteNode);
        if (fManager_c::m_nowLoopProc == fManager_c::EXECUTE) {
            mIsNotDeferred = true;
        } else {
            fManager_c::m_executeManage.addNode(&mMng.mExecuteNode);
            fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
            mLifecycleState = 1;
        }
    } else if (state == ERROR) {
        deleteRequest();
    }
}

int fBase_c::doDelete() {
    return 1;
}
int fBase_c::createPack() {
    return commonPack(&fBase_c::create, &fBase_c::preCreate, &fBase_c::postCreate);
}
bool fBase_c::preDelete() {
    if (mpUnusedHelper != nullptr) {
        if (!mpUnusedHelper->LoadOnlyOne()) {
            return false;
        }
    }
    fBase_c *child = getConnectChild();
    if (child != nullptr) {
        return false;
    }
    return true;
}
void fBase_c::postDelete(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        fManager_c::m_connectManage.removeTreeNode(&mMng.mConnectNode);
        fManager_c::m_searchManage[mMng.getSearchTableNum()].remove(&mMng.mSearchNode);
        fManager_c::m_deleteManage.remove(&mMng.mExecuteNode);

        if (mpHeap != nullptr) {
            // TODO: decomp EGG::Heap so that virtual functions are correct
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
bool fBase_c::preExecute() {
    if (mWantsDelete || (mProcessFlags & 2)) {
        return false;
    }
    return true;
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
bool fBase_c::preDraw() {
    if (mWantsDelete || (mProcessFlags & 8)) {
        return false;
    }
    return true;
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
    if (mWantsDelete) {
        mWantsDelete = false;
        if (mLifecycleState == 1) {
            fManager_c::m_executeManage.remove(&mMng.mExecuteNode);
            fManager_c::m_drawManage.remove(&mMng.mDrawNode);
        } else {
            fManager_c::m_createManage.remove(&mMng.mExecuteNode);
        }
        fManager_c::m_deleteManage.prepend(&mMng.mExecuteNode);
        mLifecycleState = 2;
        for (fTrNdBa_c *curr = mMng.mConnectNode.getChild(); curr != nullptr; curr = curr->getBrNext()) {
            curr->mpOwner->deleteRequest();
        }
    } else {
        fBase_c *parent = getConnectParent();
        if (parent != nullptr) {
            if (parent->isProcessFlag(1) || parent->isProcessFlag(2)) {
                mProcessFlags |= 2;
            } else if (isProcessFlag(2)) {
                mProcessFlags &= ~2;
            }
            
            if (parent->isProcessFlag(4) || parent->isProcessFlag(8)) {
                mProcessFlags |= 8;
            } else if (isProcessFlag(8)) {
                mProcessFlags &= ~8;
            }
        }

        if (mLifecycleState == 1) {
            fLiNdPrio_c *executeNode = &mMng.mExecuteNode;
            if (executeNode->mOrder2 != executeNode->mOrder) {
                fManager_c::m_executeManage.remove(&mMng.mExecuteNode);
                fLiNdPrio_c *executeNode2 = &mMng.mExecuteNode;
                executeNode2->mOrder = executeNode2->mOrder2;
                fManager_c::m_executeManage.addNode(executeNode2);
            }
            fLiNdPrio_c *drawNode = &mMng.mDrawNode;
            if (drawNode->mOrder2 != drawNode->mOrder) {
                fManager_c::m_drawManage.remove(&mMng.mDrawNode);
                fLiNdPrio_c *drawNode2 = &mMng.mDrawNode;
                drawNode2->mOrder = drawNode2->mOrder2;
                fManager_c::m_drawManage.addNode(drawNode2);
            }
        } else if (mLifecycleState != 2) {
            if (mIsDeferred) {
                mIsDeferred = false;
                fManager_c::m_createManage.append(&mMng.mExecuteNode);
            } else {
                if (mIsNotDeferred) {
                    mIsNotDeferred = false;
                    fManager_c::m_executeManage.addNode(&mMng.mExecuteNode);
                    fManager_c::m_drawManage.addNode(&mMng.mDrawNode);
                    mLifecycleState = 1;
                }
            }
        }
    }
    return 1;
}

void fBase_c::deleteRequest() {
    if (!mWantsDelete && mLifecycleState != 2) {
        mWantsDelete = true;
        deleteReady();
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
        newHeap = makeFrmAndUpdate(size, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
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
        newHeap = makeFrmAndUpdate(-1, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
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
        EGG::FrmHeap *largerHeap = makeFrmAndUpdate(heapSize, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
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
    deleteRequest();
    return false;
}
bool fBase_c::entryFrmHeapNonAdjust(unsigned long size, EGG::Heap *parentHeap) {
    if (mpHeap != nullptr) {
        return true;
    }
    EGG::FrmHeap *newHeap = makeFrmAndUpdate(size, parentHeap, F_BASE_HEAP_NAME, 0x20, 0);
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
    deleteRequest();
    return false;
}

bool fBase_c::createHeap() {
    return true;
}

void *fBase_c::operator new(unsigned long size) {
    void *mem = EGG::Heap::alloc(size, -4, mHeap::g_gameHeaps[0]);
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
    if (!mWantsDelete && !mIsNotDeferred && mLifecycleState == 0) {
        if (fManager_c::m_nowLoopProc == fManager_c::CREATE) {
            mIsDeferred = true;
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
        if (curr->mpOwner->mLifecycleState == 0) {
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
