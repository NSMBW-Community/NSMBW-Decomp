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
    
    inline fLiMgBa_c *getFirst() {
        return (fLiMgBa_c *) cListMg_c::getFirst();
    }
    inline fLiMgBa_c *getLast() {
        return (fLiMgBa_c *) cListMg_c::getLast();
    }
};