#include <dol/base/f_manager.hpp>
#include <dol/base/f_base.hpp>

fTrMgPTMF_c fManager_c::m_connectManage(&fBase_c::connectProc);
fLiMgPTMF_c fManager_c::m_createManage(&fBase_c::createPack);
fLiMgPTMF_c fManager_c::m_executeManage(&fBase_c::executePack);
fLiMgPTMF_c fManager_c::m_drawManage(&fBase_c::drawPack);
fLiMgPTMF_c fManager_c::m_deleteManage(&fBase_c::deletePack);
fLiMgBa_c fManager_c::m_searchManage[8];

u32 fManager_c::m_StopProcInf;
fManager_c::LOOP_PROC_e fManager_c::m_nowLoopProc = CONNECT;

int fManager_c::getSearchTableNum() {
    return ((fBase_c *) (mSearchNode.mpOwner))->mUniqueID & 7;
}

fBase_c *fManager_c::searchBaseByID(fBaseID_e id) {
    const fLiNdBa_c *res = m_searchManage[id & 7].searchNodeByID(id);
    if (res != nullptr) {
        return res->mpOwner;
    }
    return 0;
}

fBase_c *fManager_c::searchBaseByProfName(Profile profName, const fBase_c *parent) {
    const fTrNdBa_c *connectNode;
    if (parent != nullptr) {
        connectNode = &parent->mMng.mConnectNode;
    } else {
        connectNode = nullptr;
    }
    
    const fTrNdBa_c *res = m_connectManage.searchNodeByProfName(profName, connectNode);
    if (res != nullptr) {
        return res->mpOwner;
    }
    return 0;
}

fBase_c *fManager_c::searchBaseByGroupType(u8 groupType, const fBase_c *parent) {
    const fTrNdBa_c *connectNode;
    if (parent != nullptr) {
        connectNode = &parent->mMng.mConnectNode;
    } else {
        connectNode = nullptr;
    }

    const fTrNdBa_c *res = m_connectManage.searchNodeByGroupType(groupType, connectNode);
    if (res != nullptr) {
        return res->mpOwner;
    }
    return 0;
}

void fManager_c::mainLoop() {
    if ((m_StopProcInf & 8) == 0) {
        m_nowLoopProc = DELETE;
        m_deleteManage.walkPack();
    }
    if ((m_StopProcInf & 2) == 0) {
        m_nowLoopProc = CREATE;
        m_createManage.walkPack();
    }
    if ((m_StopProcInf & 4) == 0) {
        m_nowLoopProc = EXECUTE;
        m_executeManage.walkPack();
    }
    if ((m_StopProcInf & 16) == 0) {
        m_nowLoopProc = DRAW;
        m_drawManage.walkPack();
    }
    if ((m_StopProcInf & 1) == 0) {
        m_nowLoopProc = CONNECT;
        m_connectManage.walkPack();
    }
    m_nowLoopProc = NOTHING;
}