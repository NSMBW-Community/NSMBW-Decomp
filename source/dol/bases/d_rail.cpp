#include <game/bases/d_rail.hpp>
#include <game/bases/d_s_stage.hpp>

sRailInfoData *dRail_c::getRailInfoP(u8 id) {
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sRailInfoData *curr = cdFile->mpRails;
    for (u32 i = 0; i < cdFile->mRailCount; i++) {
        if (curr->mID == id) {
            return curr;
        }
        curr++;
    }
    return nullptr;
}

bool dRail_c::calculate(u8 id, mVec2_c *v, u32 a, u32 b, u16 c, bool d) {
    m_1c = 0;
    m_04 = nullptr;
    sRailInfoData *railInfo = getRailInfoP(id);
    if (railInfo == nullptr) {
        return false;
    }
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sRailNodeData *railNode = &cdFile->mpRailNodes[railInfo->mNodeIdx];
    if (railInfo->m_04 <= c) {
        c = 0;
    }
    m_1c = railInfo->m_04;
    m_10 = v;
    m_10->x = railNode[c].m_00;
    m_10->y = -railNode[c].m_02;
    m_14 = a;
    m_18 = b;
    m_04 = railNode;
    m_1e = c;
    m_20 = c;
    m_22 = railNode->m_0c;
    m_2a = d;
    m_26 = railInfo->m_06;
    m_28 = railNode[c].m_0e;
    calcNextPoint();
    calcTargetPos();
    calcSpeed();
    return true;
}
