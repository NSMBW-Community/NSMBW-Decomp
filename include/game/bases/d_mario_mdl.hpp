#pragma once

#include <game/bases/d_player_model.hpp>

namespace d3d {
    void setMatCullMode(m3d::bmdl_c*, int, _GXCullMode);
}

class dMarioMdl_c : public dPlayerMdl_c {
public:
    dMarioMdl_c(u8 modelType);

    void _calc() override;
    void play() override;
    void draw() override;
    void setColorType(u8 type) override;
    void setTexAnmType(dPyMdlBase_c::TexAnmType_e type) override; // useless override
    bool setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int) override;
    void FUN_800d4750(int mode) override;
    float getAimMotionShareScale() override;
    void updateBonusCap() override;

    void initialize();
    void createPlayerModel();
    void fn_800cab00(int); ///< @unofficial

    u32 m_820;
    m3d::smdl_c mCapModels[2];
    m3d::anmTexPat_c mCapPatAnm;
    u32 m_868;
    u32 m_86c;
};
