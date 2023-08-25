#pragma once

#include <types.h>
#include <game/framework/f_list_mg.hpp>
#include <game/framework/f_list_nd_prio.hpp>

class fBase_c;

/// @brief A base list made of fLiNdPrio_c nodes, with a reference to a process function.
/// @ingroup framework
/// @unofficial
class fLiMgPTMF_c : public fLiMgBa_c {
public:

    /// @brief Constructs a new list.
    /// @param procFunc The process function, or @p nullptr.
    fLiMgPTMF_c(int (fBase_c::*procFunc)()) : mpProcFunc(procFunc) {}

    /**
     * @brief Adds a node to the list, according to its priority.
     *
     * @param node The node to insert.
     * @return If the operation was successful.
     */
    bool addNode(fLiNdPrio_c *node);

    /// @brief Calls the @ref ::mpProcFunc "process function" on each base in the list.
    /// @return Always returns true.
    bool walkPack();

    fLiNdPrio_c *getFirst() const {
        return (fLiNdPrio_c *) fLiMgBa_c::getFirst();
    }

    fLiNdPrio_c *getLast() const {
        return (fLiNdPrio_c *) fLiMgBa_c::getLast();
    }

private:
    int (fBase_c::*mpProcFunc)(); ///< The process function for the list.
};
