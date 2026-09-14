#include <game/bases/d_center_save_mng.hpp>

dCenterSaveMng_c *dCenterSaveMng_c::m_instance;
cLineMg_c dCenterSaveMng_c::ms_linkManager;

dCenterSaveMng_c::dCenterSaveMng_c() {
    m_instance = this;
}

dCenterSaveMng_c::~dCenterSaveMng_c() {
    allDoDelete();
    m_instance = nullptr;
}

void dCenterSaveMng_c::allDoDelete() {
    Link_c *link;

    Link_c *curr = (Link_c *) ms_linkManager.getFirst();

    while (curr != nullptr) {
        link = curr->mpSelf;
        curr = (Link_c *) curr->getNext();

        delete link;
    }
}

bool dCenterSaveMng_c::center_check(const mVec3_c *pos, u8 id, short *outValue) {
    Link_c *link;

    Link_c *curr = (Link_c *) ms_linkManager.getFirst();
    bool found = false;
    *outValue = 0;

    while (curr != nullptr) {
        link = curr->mpSelf;
        curr = (Link_c *) curr->getNext();

        if (pos->x == link->getX() && pos->y == link->getY() && id == link->mID) {
            *outValue = link->mRotation;
            found = true;
            break;
        }
    }

    return found;
}

void dCenterSaveMng_c::center_entry(const mVec3_c *pos, u8 id, short value) {
    Link_c *link;

    Link_c *curr = (Link_c *) ms_linkManager.getFirst();
    bool found = false;

    while (curr != nullptr) {
        link = curr->mpSelf;
        curr = (Link_c *) curr->getNext();

        if (pos->x == link->getX() && pos->y == link->getY() && id == link->mID) {
            link->mRotation = value;
            found = true;
            break;
        }
    }

    if (!found) {
        Link_c *newEntry = new Link_c();
        if (newEntry != nullptr) {
            newEntry->mPos = *pos;
            newEntry->mID = id;
            newEntry->mRotation = value;
        }
    }
}
