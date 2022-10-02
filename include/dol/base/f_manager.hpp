#pragma once

#include <dol/base/f_base_id.hpp>
#include <dol/base/f_list_mg_ptmf.hpp>
#include <dol/base/f_list_mg.hpp>
#include <dol/base/f_list_nd.hpp>
#include <dol/base/f_list_nd_prio.hpp>
#include <dol/base/f_tree_mg_ptmf.hpp>
#include <dol/base/f_tree_nd.hpp>
#include <dol/base/f_profile.hpp>

class fBase_c;

/// @brief A class that manages the execution of the bases.
class fManager_c {
public:
    fTrNdBa_c mConnectNode; ///< The node in ::m_connectManage.
    fLiNdPrio_c mExecuteNode; ///< The node in ::m_executeManage.
    fLiNdPrio_c mDrawNode; ///< The node in ::m_drawManage.
    fLiNdBa_c mSearchNode; ///< The node in ::m_searchManage.

    /**
     * @brief Construct a new fManager_c.
     * 
     * @param owner The owner of the manager.
     */
    fManager_c(fBase_c *owner) :
        mConnectNode(owner),
        mExecuteNode(owner),
        mDrawNode(owner),
        mSearchNode(owner) {}

    /// @brief Executes the five processes on all bases in the manager lists.
    static void mainLoop();
    
    /// @brief Get the index of the search list corresponding to the owner of the manager.
    int getSearchTableNum();
    
    static fBase_c *searchBaseByID(fBaseID_e id); ///< Search for a base with a given ID.
    static fBase_c *searchBaseByProfName(ProfileName profID, const fBase_c *parent); ///< Search for a base with a given profile name.
    static fBase_c *searchBaseByGroupType(unsigned char groupType, const fBase_c *parent); ///< Search for a base with a given group type.

    static fTrMgPTMF_c m_connectManage; ///< A tree that connects all loaded bases.
    static fLiMgPTMF_c m_createManage; ///< A list that is used to create the bases which are waiting for creation.
    static fLiMgPTMF_c m_executeManage; ///< A list that is used to call fBase_c::executePack on every base.
    static fLiMgPTMF_c m_deleteManage; ///< A list that is used to delete the bases which are waiting for deletion.
    static fLiMgPTMF_c m_drawManage; ///< A list that is used to call fBase_c::drawPack on every base.
    static fLiMgBa_c m_searchManage[8]; ///< An array of lists that is used to search for bases.
    
    /// @brief Flags used for bitmasks of the different processes. @see ::LOOP_PROC_e
    enum PROC_FLAGS {
        PROC_FLAG_CONNECT = 1,
        PROC_FLAG_CREATE = 2,
        PROC_FLAG_EXECUTE = 4,
        PROC_FLAG_DELETE = 8,
        PROC_FLAG_DRAW = 16
    };

    static u32 m_StopProcInf; ///< Which processes should be executed.
    
    /// @brief The processes for fManager_c. @see ::PROC_FLAGS
    enum LOOP_PROC_e {
        NOTHING, CONNECT, CREATE, EXECUTE, DELETE, DRAW
    };
    static LOOP_PROC_e m_nowLoopProc; ///< The process the main loop is currently in.
};
