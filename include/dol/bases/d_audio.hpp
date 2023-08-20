#pragma once
#include <dol/framework/f_profile.hpp>

namespace dAudio {
    void requestStartScene(ProfileName prof);
    void prepareSceneSnd(); ///< @unofficial
    void destroySceneSnd(); ///< @unofficial
    void setNextScene(ProfileName prof, unsigned long bgmIndex);
};
