#pragma once

#include <types.h>
#include <dol/profile/profile.hpp>
#include <dol/cLib/c_list.hpp>
#include <dol/base/f_base_id.hpp>
#include <dol/base/f_list_nd.hpp>

class fBase_c;

class fLiMgBa_c : public cListMg_c {
public:
    int countNodeByProfName(Profile profID) const;
    const fLiNdBa_c *searchNodeByID(fBaseID_e id) const;
    
    inline fLiNdBa_c *getFirst() {
        return (fLiNdBa_c *) cListMg_c::getFirst();
    }
    inline fLiNdBa_c *getLast() {
        return (fLiNdBa_c *) cListMg_c::getLast();
    }
};