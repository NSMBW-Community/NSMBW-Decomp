#pragma once
#include <types.h>
#include <dol/framework/f_profile.hpp>
#include <dol/cLib/c_list.hpp>
#include <dol/framework/f_base_id.hpp>
#include <dol/framework/f_list_nd.hpp>

class fBase_c;

/// @brief A list of fLiNdBa_c nodes.
/// @ingroup framework dol
class fLiMgBa_c : public cListMg_c {
public:
    /**
     * @brief Counts the number of nodes of a given profile in this list.
     *
     * @param profName The profile name.
     * @return How many nodes were found.
     */
    int countNodeByProfName(ProfileName profName) const;

    /**
     * @brief Finds a node in this list with the given ID.
     *
     * @param id The ID to search for.
     * @return The found node, or @p nullptr if none were found.
     */
    const fLiNdBa_c *searchNodeByID(fBaseID_e id) const;

    inline fLiNdBa_c *getFirst() const {
        return (fLiNdBa_c *) cListMg_c::getFirst();
    }

    inline fLiNdBa_c *getLast() const {
        return (fLiNdBa_c *) cListMg_c::getLast();
    }
};
