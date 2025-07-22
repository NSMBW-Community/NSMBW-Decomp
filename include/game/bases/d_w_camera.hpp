#include <game/mLib/m_sphere.hpp>
#include <game/bases/d_wm_lib.hpp>

class dWmViewClip_c {
public:
    void set( float, float, float, float, const mMtx_c& );
    bool CheckClipSphere(const mSphere_c *);
};

class dWCamera_c {
public:
    char pad[0x4f8];
    dWmViewClip_c mViewClip; ///< The camera's view clip.

    static dWCamera_c *m_instance; ///< The instance of the camera.
};
