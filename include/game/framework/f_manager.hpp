#pragma once
#include <game/framework/f_base_id.hpp>
#include <game/framework/f_list_mg_ptmf.hpp>
#include <game/framework/f_list_mg.hpp>
#include <game/framework/f_list_nd.hpp>
#include <game/framework/f_list_nd_prio.hpp>
#include <game/framework/f_tree_mg_ptmf.hpp>
#include <game/framework/f_tree_nd.hpp>
#include <game/framework/f_profile.hpp>

#define GET_PROC_FLAG(proc) BIT_FLAG((proc)-1)

class fBase_c;

/// @brief Manages the execution of base operations.
/// @ingroup framework
class fManager_c {
private:

    /// @brief The operation types.
    enum LOOP_PROC_e {
        NONE, CONNECT, CREATE, EXECUTE, DELETE, DRAW
    };

    /// @brief The operation disable flags, induced from ::LOOP_PROC_e.
    enum PROC_FLAGS_e {
        PROC_FLAG_NONE    = GET_PROC_FLAG(NONE),
        PROC_FLAG_CONNECT = GET_PROC_FLAG(CONNECT),
        PROC_FLAG_CREATE  = GET_PROC_FLAG(CREATE),
        PROC_FLAG_EXECUTE = GET_PROC_FLAG(EXECUTE),
        PROC_FLAG_DELETE  = GET_PROC_FLAG(DELETE),
        PROC_FLAG_DRAW    = GET_PROC_FLAG(DRAW)
    };

    /// @brief Constructs a new manager.
    /// @param owner The manager's owner.
    fManager_c(fBase_c *owner) :
        mConnectNode(owner),
        mMainNode(owner),
        mDrawNode(owner),
        mSearchNode(owner) {}

    /// @brief Gets the index of the search list the owning base was added to. See ::m_searchManage.
    int getSearchTableNum();

public:
    /// @brief Executes the currently enabled operations on all the bases in the respective lists.
    /// @details As this function indirectly executes most of the game-specific code, it can be
    /// considered as a de facto @p main method.
    static void mainLoop();

    /// @brief Searches for a base with the given ID.
    /// @details [This calls fLiMgBa_c::searchNodeByID internally].
    static fBase_c *searchBaseByID(fBaseID_e id);

    /// @brief Searches for a base with a given profile name, optionally under a given parent.
    /// @details [This calls fTrMgBa_c::searchNodeByProfName internally].
    static fBase_c *searchBaseByProfName(ProfileName profID, const fBase_c *parent);

    /// @brief Searches for a base with a given group type, optionally under a given parent.
    /// @details [This calls fTrMgBa_c::searchNodeByGroupType internally].
    static fBase_c *searchBaseByGroupType(unsigned char groupType, const fBase_c *parent);

private:
    fTrNdBa_c mConnectNode; ///< The node in the @ref ::m_connectManage "connect tree".

    /// @brief The node in the @ref ::m_createManage "create", @ref ::m_executeManage "execute" or
    /// @ref ::m_deleteManage "delete" list.
    fLiNdPrio_c mMainNode;
    fLiNdPrio_c mDrawNode; ///< The node in the @ref ::m_drawManage "draw list".
    fLiNdBa_c mSearchNode; ///< The node in the @ref ::m_searchManage "search lists".

    static fTrMgPTMF_c m_connectManage; ///< A tree that connects all loaded bases.
    static fLiMgPTMF_c m_createManage; ///< A list of all the bases scheduled for creation.
    static fLiMgPTMF_c m_executeManage; ///< A list of all the bases scheduled for execution.
    static fLiMgPTMF_c m_deleteManage; ///< A list of all the bases scheduled for deletion.
    static fLiMgPTMF_c m_drawManage; ///< A list of all the bases scheduled for drawing.

    /// @brief An array of lists used for base lookup.
    /// @details Bases are distributed across the lists in a round-robin fashion, allowing bases to be
    /// located more efficiently.
    static fLiMgBa_c m_searchManage[8];

    /// @brief The operations which should be globally skipped this frame.
    /// @unused
    /// @details Value is a combination of ::PROC_FLAGS_e.
    static u32 m_StopProcInf;

    /// @brief The current operation being globally executed. See ::mainLoop.
    /// @details The list for this operation cannot be updated until the operation has finished executing.
    /// @hideinitializer
    static LOOP_PROC_e m_nowLoopProc;

    friend class fBase_c;
};
