#pragma once

#include <game/bases/d_player_model.hpp>

class dKinopioMdl_c : public dPlayerMdl_c {
public:
    dKinopioMdl_c(u8 modelType);
    ~dKinopioMdl_c() override;

    void initialize() override;
    void play() override;
    void _calc() override;
    void draw() override;
    void setColorType(u8 type) override;
    bool setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int) override;
    bool setPersonalRideAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr) override;
    void setTexAnmType(TexAnmType_e type) override;
    void callbackTimingA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResNode resNode) override;
    float getAimMotionShareScale() override;
    void createPlayerModel() override;

    m3d::anmTexPat_c &getBodyTexAnm();

    m3d::anmTexPat_c mAnimPropeller;
    m3d::anmTexPat_c mAnimPenguin;
};
