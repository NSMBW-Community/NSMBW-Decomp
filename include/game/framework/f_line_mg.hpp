#pragma once
#include <types.h>
#include <game/framework/f_profile.hpp>
#include <game/cLib/c_line.hpp>
#include <game/framework/f_base_id.hpp>
#include <game/framework/f_line_nd.hpp>

class fBase_c;

/// @brief A base list, made of fLiNdBa_c nodes.
/// @ingroup framework
/// @decompnote{The class name stands for "Line Manager (for) Bases".}
class fLiMgBa_c : public cLineMg_c {
public:

    /**
     * @brief Counts the number of bases using the given profile name.
     *
     * @param profName The profile name.
     * @return How many bases were found.
     */
    int countNodeByProfName(ProfileName profName) const;

    /**
     * @brief Searches for a base with the given ID.
     *
     * @param id The ID to search for.
     * @return The node belonging to such base, or @p nullptr.
     */
    const fLiNdBa_c *searchNodeByID(fBaseID_e id) const;

    fLiNdBa_c *getFirst() const {
        return (fLiNdBa_c *) cLineMg_c::getFirst();
    }

    fLiNdBa_c *getLast() const {
        return (fLiNdBa_c *) cLineMg_c::getLast();
    }
};

/// @brief A base list made of fLiMgBa_c nodes, with a reference to a process function.
/// @ingroup framework
class fLiMgBaFu_c : public fLiMgBa_c {
public:

    /// @brief Constructs a new list.
    /// @param procFunc The process function, or @p nullptr.
    fLiMgBaFu_c(int (fBase_c::*procFunc)()) : mpProcFunc(procFunc) {}

    /// @brief Calls the @ref mpProcFunc "process function" on each base in the list.
    /// @return Always returns true.
    bool lineListProc();

private:
    int (fBase_c::*mpProcFunc)(); ///< The process function for the list.
};

/// @brief A base list like fLiMgBa_c, but with fLiNdBaPr_c nodes for priority-based ordering.
/// @ingroup framework
class fLiMgBaFuPr_c : public fLiMgBaFu_c {
public:

    /// @brief Constructs a new list.
    /// @param procFunc The process function, or @p nullptr.
    fLiMgBaFuPr_c(int (fBase_c::*procFunc)()) : fLiMgBaFu_c(procFunc) {}

    /**
     * @brief Adds a node to the list, according to its priority.
     *
     * @param node The node to insert.
     * @return If the operation was successful.
     */
    bool insertLineNodePriority(fLiNdBaPr_c *node);

    fLiNdBaPr_c *getFirst() const {
        return (fLiNdBaPr_c *) fLiMgBa_c::getFirst();
    }

    fLiNdBaPr_c *getLast() const {
        return (fLiNdBaPr_c *) fLiMgBa_c::getLast();
    }
};
