#pragma once

class fTrNdBa_c;
class fLiNdBa_c;
class fLiNdBaPrio_c;
class fLiMgPTMF_c;
class fTrMgPTMF_c;

class fManager_c {
    fTrNdBa_c *mConnectNode;
    fLiNdBaPrio_c *mExecuteNode;
    short mExecuteOrder;
    fLiNdBaPrio_c *mDrawNode;
    short mDrawOrder;
    fLiNdBa_c *mSearchNode;

public:
    u32 getSearchTableNum();

    static fLiMgPTMF_c m_connectManage;
    static fLiMgPTMF_c m_createManage;
    static fLiMgPTMF_c m_executeManage;
    static fLiMgPTMF_c m_deleteManage;
    static fLiMgPTMF_c m_drawManage;
    static fTrMgPTMF_c m_searchManage[8];
};
