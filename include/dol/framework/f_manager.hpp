#pragma once
#include <dol/framework/f_base_id.hpp>
#include <dol/framework/f_list_mg_ptmf.hpp>
#include <dol/framework/f_list_mg.hpp>
#include <dol/framework/f_list_nd.hpp>
#include <dol/framework/f_list_nd_prio.hpp>
#include <dol/framework/f_tree_mg_ptmf.hpp>
#include <dol/framework/f_tree_nd.hpp>
#include <dol/framework/f_profile.hpp>

class fBase_c;

/// @brief A class that manages the execution of the bases.
class fManager_c {
public:

    /// @brief Flags used for bitmasks of the different processes. @see ::LOOP_PROC_e
    enum PROC_FLAGS {
        PROC_FLAG_CONNECT = 1,
        PROC_FLAG_CREATE = 2,
        PROC_FLAG_EXECUTE = 4,
        PROC_FLAG_DELETE = 8,
        PROC_FLAG_DRAW = 16
    };

    /// @brief The processes for fManager_c. @see ::PROC_FLAGS
    enum LOOP_PROC_e {
        NOTHING, CONNECT, CREATE, EXECUTE, DELETE, DRAW
    };

    fTrNdBa_c mConnectNode; ///< The node in ::m_connectManage.
    fLiNdPrio_c mExecuteNode; ///< The node in ::m_executeManage.
    fLiNdPrio_c mDrawNode; ///< The node in ::m_drawManage.
    fLiNdBa_c mSearchNode; ///< The node in ::m_searchManage.

    /// @brief Constructs a new fManager_c.
    /// @param owner The manager's owner.
    fManager_c(fBase_c *owner) :
        mConnectNode(owner),
        mExecuteNode(owner),
        mDrawNode(owner),
        mSearchNode(owner) {}
    
    /// @brief Gets the index of the search list corresponding to the owner of the manager.
    /// @see m_searchManage
    int getSearchTableNum();
    
    /// @brief Executes the currently enabled processes on all the bases in the respective lists.
    static void mainLoop();

    /// @brief Searches for a base with the given ID.
    static fBase_c *searchBaseByID(fBaseID_e id);

    /// @brief Searches for a base with a given profile name, optionally under a given parent.
    static fBase_c *searchBaseByProfName(ProfileName profID, const fBase_c *parent);

    /// @brief Searches for a base with a given group type, optionally under a given parent.
    static fBase_c *searchBaseByGroupType(unsigned char groupType, const fBase_c *parent);

    static fTrMgPTMF_c m_connectManage; ///< A tree that connects all loaded bases.
    static fLiMgPTMF_c m_createManage; ///< A list of all the bases scheduled for creation.
    static fLiMgPTMF_c m_executeManage; ///< A list of all the bases scheduled for execution.
    static fLiMgPTMF_c m_deleteManage; ///< A list of all the bases scheduled for deletion.
    static fLiMgPTMF_c m_drawManage; ///< A list of all the bases scheduled for drawing.

    /// @brief An array of lists that is used to search for bases.
    /// @details Bases are distributed evenly between each entry in the array.
    /// This allows for more efficient base lookups.
    static fLiMgBa_c m_searchManage[8];

    static u32 m_StopProcInf; ///< Which processes should be executed this frame.
    static LOOP_PROC_e m_nowLoopProc; ///< The process ::mainLoop is currently in.
};
