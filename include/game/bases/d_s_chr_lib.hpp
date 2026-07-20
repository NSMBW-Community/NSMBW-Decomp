#include <game/mLib/m_3d.hpp>

class dsChrLib {
public:
    static void releaseAnim(m3d::anmChr_c *anim);
    static void bindAnimToNodeList(m3d::bmdl_c *model, m3d::anmChr_c *anim, int nodeCount, const char **nodeNames, nw4r::g3d::AnmObjChr::BindOption bindOption);
};
