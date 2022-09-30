#pragma once

#include <types.h>
#include <dol/profile/profile.hpp>
#include <dol/cLib/c_list.hpp>
#include <dol/base/f_base_id.hpp>
#include <dol/base/f_list_nd.hpp>

class fBase_c;

class fLiMgBa_c : public cListMg_c {
public:
    fLiMgBa_c() {
        mpFirst = nullptr;
        mpLast = nullptr;
    }

    int countNodeByProfName(Profile profID) const;
    const fLiNdBa_c *searchNodeByID(fBaseID_e id) const;
};