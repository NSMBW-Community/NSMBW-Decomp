#pragma once

#include <game/bases/d_player_model.hpp>

namespace d3d {
    void setMatCullMode(m3d::bmdl_c*, int, _GXCullMode);
}

class dMarioMdl_c : public dPlayerMdl_c {
public:
    enum HeadType_e {
        TYPE_0,
        TYPE_1
    };

    dMarioMdl_c(u8 modelType);

    void setHeadID(HeadType_e type);
    void initialize() override;
    void play() override;
    void _calc() override;
    void draw() override;
    void setColorType(u8 type) override;
    void setTexAnmType(dPyMdlBase_c::TexAnmType_e type) override; // useless override
    bool setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int) override;
    void updateBonusCap() override;
    void FUN_800d4750(int mode) override;
    float getAimMotionShareScale() override;
    void createPlayerModel() override;

    u32 m_820;
    m3d::smdl_c mCapModels[2];
    m3d::anmTexPat_c mCapPatAnm;
    u32 m_868;
    u32 m_86c;
};
