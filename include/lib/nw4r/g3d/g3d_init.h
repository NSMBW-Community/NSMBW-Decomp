#ifndef NW4R_G3D_INIT_H
#define NW4R_G3D_INIT_H
#include <nw4r/types_nw4r.h>

#include <revolution/OS.h>

namespace nw4r {
namespace g3d {

void G3dInit(bool enableLockedCache);
void G3dReset();

} // namespace g3d
} // namespace nw4r

#endif
