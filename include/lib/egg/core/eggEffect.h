#pragma once

#include <nw4r/math.h>

namespace EGG {

class Effect {
public:
    enum ERecursive {};

    Effect();
    virtual ~Effect();
    virtual void create();
    virtual void fade();
    virtual void followFade();
    virtual void kill();
    virtual void setDisableCalc(bool);
    virtual void setDisableDraw(bool);
    virtual void setDisableCalcDraw(bool);
    virtual void setLife(unsigned short, EGG::Effect::ERecursive);
    virtual void setEmitRatio(float, EGG::Effect::ERecursive);
    virtual void setEmitInterval(unsigned short, EGG::Effect::ERecursive);
    virtual void setEmitEmitDiv(unsigned short, EGG::Effect::ERecursive);
    virtual void setInitVelocityRandom(s8, EGG::Effect::ERecursive);
    virtual void setPowerYAxis(float, EGG::Effect::ERecursive);
    virtual void setPowerRadiationDir(float, EGG::Effect::ERecursive);
    virtual void setPowerSpecDir(float, EGG::Effect::ERecursive);
    virtual void setPowerSpecDirAdd(float, EGG::Effect::ERecursive);
    virtual void setSpecDir(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setSpecDirAdd(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setVelocity(const nw4r::math::VEC3&);
    virtual void setColor(unsigned char, unsigned char, unsigned char, unsigned char, EGG::Effect::ERecursive);
    virtual void vf5c();
    virtual void vf60();
    virtual void setDefaultParticleSize(nw4r::math::VEC2&, EGG::Effect::ERecursive);
    virtual void setParticleScale(nw4r::math::VEC2&, EGG::Effect::ERecursive);
    virtual void setDefaultParticleRotate(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setParticleRotate(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setEmitterSize(const nw4r::math::VEC3&, bool, EGG::Effect::ERecursive);
    virtual void setLocalScale(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setDynamicsScale(const nw4r::math::VEC3&, const nw4r::math::VEC2*);
    virtual void setScale(float);
    virtual void setScale(const nw4r::math::VEC3&);
    virtual void setPos(const nw4r::math::VEC3&);
    virtual void setMtx(const nw4r::math::MTX34&);
    virtual void setPtclAnim(int, bool);
    virtual void update();
    virtual void reset();

    void getEffect() const;
    void getRootEmitter() const;

    u8 mPad[0x7c];
};

} // namespace EGG
