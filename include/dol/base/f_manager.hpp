#pragma once

#include <dol/base/f_base_id.hpp>
#include <dol/base/f_list_mg_ptmf.hpp>
#include <dol/base/f_list_mg.hpp>
#include <dol/base/f_list_nd.hpp>
#include <dol/base/f_list_nd_prio.hpp>
#include <dol/base/f_tree_mg_ptmf.hpp>
#include <dol/base/f_tree_nd.hpp>
#include <dol/profile/profile.hpp>

class fBase_c;

class fManager_c {
public:
    fTrNdBa_c mConnectNode;
    fLiNdPrio_c mExecuteNode;
    fLiNdPrio_c mDrawNode;
    fLiNdBa_c mSearchNode;

    fManager_c(fBase_c *owner) :
        mConnectNode(owner),
        mExecuteNode(owner),
        mDrawNode(owner),
        mSearchNode(owner) {}

    void mainLoop();
    static fBase_c *searchBaseByID(fBaseID_e id);
    static fBase_c *searchBaseByProfName(Profile profID, const fBase_c *parent);
    static fBase_c *searchBaseByGroupType(unsigned char groupType, const fBase_c *parent);
    int getSearchTableNum();

    static fTrMgPTMF_c m_connectManage;
    static fLiMgPTMF_c m_createManage;
    static fLiMgPTMF_c m_executeManage;
    static fLiMgPTMF_c m_deleteManage;
    static fLiMgPTMF_c m_drawManage;
    static fLiMgBa_c m_searchManage[8];

    static u32 m_StopProcInf;
    
    enum LOOP_PROC_e {
        NOTHING, CONNECT, CREATE, EXECUTE, DELETE, DRAW
    };
    static LOOP_PROC_e m_nowLoopProc;
};
