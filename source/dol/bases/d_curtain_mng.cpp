#include <game/bases/d_curtain_mng.hpp>

void dCurtainMng_c::CurtainInfoAllClear() {
    for (int i = 0; i < ARRAY_SIZE(mCurtainInfo); i++) {
        for (int j = 0; j < ARRAY_SIZE(mCurtainInfo[i]); j++) {

            CurtainInfo_c* info = &mCurtainInfo[i][j];
            info->mUnk00 = 0xFFFF;

            for (int k = 0; k < ARRAY_SIZE(info->mUnk04); k++) {
                info->mUnk04[k] = 0.0f;
            }
        }
    }
}
