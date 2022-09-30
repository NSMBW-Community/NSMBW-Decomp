#pragma once

#include <types.h>
#include <dol/profile/profile.hpp>
#include <dol/cLib/c_list.hpp>

class fLiMgBa_c : public cListMg_c {
public:
    fLiMgBa_c() {
        mpFirst = nullptr;
        mpLast = nullptr;
    }
};