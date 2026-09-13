#include <game/bases/d_center_save_mng.hpp>

dCenterSaveMng_c *dCenterSaveMng_c::m_instance = nullptr;
cLineMg_c dCenterSaveMng_c::ms_linkManager;

dCenterSaveMng_c::dCenterSaveMng_c() {
    m_instance = this;
}

dCenterSaveMng_c::~dCenterSaveMng_c() {
    allDoDelete();
    m_instance = nullptr;
}

void dCenterSaveMng_c::allDoDelete() {
    Entry *data;

    cLineNd_c *node = ms_linkManager.getFirst();
    while (node != nullptr) {
        data = ((Entry *)node)->mpData;
        node = node->getNext();

        if (data != nullptr) {
            ms_linkManager.removeLineNode(data);
            operator delete(data);
        }
    }
}

int dCenterSaveMng_c::center_check(const mVec3_c *pos, unsigned char param, short *outValue) {
    Entry *entry;

    cLineNd_c *node = ms_linkManager.getFirst();
    int found = 0;
    *outValue = found;

    while (node != nullptr) {
        entry = ((Entry *)node)->mpData;
        node = node->getNext();

        f32 entryX = entry->mPos.x;
        if (pos->x == entryX) {
            f32 entryY = entry->mPos.y;
            if (pos->y == entryY) {
                if (param == entry->mParam) {
                    *outValue = entry->mValue;
                    found = 1;
                    break;
                }
            }
        }
    }

    return found;
}

void dCenterSaveMng_c::center_entry(const mVec3_c *pos, unsigned char param, short value) {
    Entry *entry;

    cLineNd_c *node = ms_linkManager.getFirst();
    int found = 0;

    while (node != nullptr) {
        entry = ((Entry *)node)->mpData;
        node = node->getNext();

        f32 entryX = entry->mPos.x;
        if (pos->x == entryX) {
            f32 entryY = entry->mPos.y;
            if (pos->y == entryY) {
                if (param == entry->mParam) {
                    entry->mValue = value;
                    found = 1;
                    break;
                }
            }
        }
    }

    if (!found) {
        Entry *newEntry = new Entry();
        if (newEntry != nullptr) {
            newEntry->mPos.x = pos->x;
            newEntry->mPos.y = pos->y;
            newEntry->mPos.z = pos->z;
            newEntry->mParam = param;
            newEntry->mValue = value;
        }
    }
}
