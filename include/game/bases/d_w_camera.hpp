#include <game/mLib/m_sphere.hpp>
#include <game/bases/d_wm_lib.hpp>

/// @unofficial
class dWUnknownClass {
public:
    bool intersectSphere(const mSphere_c *); ///< @unofficial
};

class dWCamera_c {
public:
    char pad[0x4f8];
    dWUnknownClass mUnk;

    static dWCamera_c *m_instance; ///< The instance of the camera.
};
