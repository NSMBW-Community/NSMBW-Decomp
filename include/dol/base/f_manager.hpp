#pragma once

#include <dol/base/f_list_mg_ptmf.hpp>
#include <dol/base/f_list_mg.hpp>

class fTrNdBa_c;
class fLiNdPrio_c;
class fLiNdBa_c;

class fManager_c {
    fTrNdBa_c *mConnectNode;
    fLiNdPrio_c *mExecuteNode;
    short mExecuteOrder;
    fLiNdPrio_c *mDrawNode;
    short mDrawOrder;
    fLiNdBa_c *mSearchNode;

public:
    u32 getSearchTableNum();

    static fLiMgPTMF_c m_connectManage;
    static fLiMgPTMF_c m_createManage;
    static fLiMgPTMF_c m_executeManage;
    static fLiMgPTMF_c m_deleteManage;
    static fLiMgPTMF_c m_drawManage;
    static fLiMgBa_c m_searchManage[8];
};
