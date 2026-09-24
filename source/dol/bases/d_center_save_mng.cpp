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
    Entry_c *link;
    Entry_c *curr = (Entry_c *) ms_linkManager.getFirst();

    while (curr != nullptr) {
        link = curr->mpSelf;
        curr = (Entry_c *) curr->getNext();
        delete link;
    }
}

bool dCenterSaveMng_c::center_check(const mVec3_c *pos, u8 id, short *outRotation) {
    Entry_c *link;
    Entry_c *curr = (Entry_c *) ms_linkManager.getFirst();
    bool found = false;
    *outRotation = 0;

    while (curr != nullptr) {
        link = curr->mpSelf;
        curr = (Entry_c *) curr->getNext();

        if (pos->x == link->getX() && pos->y == link->getY() && id == link->mRotationID) {
            *outRotation = link->mRotation;
            found = true;
            break;
        }
    }

    return found;
}

void dCenterSaveMng_c::center_entry(const mVec3_c *pos, u8 id, short rotation) {
    Entry_c *link;
    Entry_c *curr = (Entry_c *) ms_linkManager.getFirst();
    bool found = false;

    while (curr != nullptr) {
        link = curr->mpSelf;
        curr = (Entry_c *) curr->getNext();

        if (pos->x == link->getX() && pos->y == link->getY() && id == link->mRotationID) {
            link->mRotation = rotation;
            found = true;
            break;
        }
    }

    if (!found) {
        Entry_c *newEntry = new Entry_c();
        if (newEntry != nullptr) {
            newEntry->mPos = *pos;
            newEntry->mRotationID = id;
            newEntry->mRotation = rotation;
        }
    }
}
