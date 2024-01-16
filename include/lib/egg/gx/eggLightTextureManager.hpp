#pragma once

#include <lib/egg/gx/eggModelEx.hpp>

namespace EGG {

class LightTextureManager {
public:
    void drawAndCaptureTexture(float, float, float, float);
    int getTextureIndex(char const *) const;
    bool replaceModelTexture(int, EGG::ModelEx *) const;
};

} // namespace EGG
