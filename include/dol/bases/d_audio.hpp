#pragma once
#include <dol/framework/f_profile.hpp>

namespace dAudio {
    void requestStartScene(ProfileName prof);
    void prepareSceneSnd(); ///< @note Unofficial name.
    void destroySceneSnd(); ///< @note Unofficial name.
    void setNextScene(ProfileName prof, unsigned long bgmIndex);
};
