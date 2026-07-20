#include <game/mLib/m_3d.hpp>

/// @unofficial
class dUnkAnimClass_c {
public:
    static void ReleaseAnim(m3d::banm_c &anim);
    static void BindNodesByName(m3d::bmdl_c &model, m3d::banm_c &anim, int nodeCount, const char **nodeNames, nw4r::g3d::AnmObjChr::BindOption bindOption);
};
