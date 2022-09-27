#pragma once

#include <types.h>
#include <dol/profile/profile.hpp>
#include <dol/cLib/c_list.hpp>
#include <dol/base/f_base.hpp>

class fLiNdBa_c : public cListNd_c {

};

class fLiMgBa_c {
private:
    fLiNdBa_c *mpTail;
    fLiNdBa_c *mpHead;

public:
    fLiMgBa_c() : mpTail(nullptr), mpHead(nullptr) {}
};

// Unofficial name
class fLiNdPrio_c : fLiNdBa_c {
public:
    short mPriority;
};

// Unofficial name
class fLiMgPTMF_c : fLiMgBa_c {
public:
    void (fBase_c::*mpPack)(void *);

    void walkPack();
};