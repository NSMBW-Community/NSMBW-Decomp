#ifndef NW4R_EF_ANIM_CURVE_H
#define NW4R_EF_ANIM_CURVE_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ef {

// Forward declarations
class Particle;

void AnimCurveExecuteU8(u8* pCmdList, u8* pTarget, ulong tick, u16 seed,
                        ulong life);

void AnimCurveExecuteF32(u8* pCmdList, f32* pTarget, ulong tick, u16 seed,
                         ulong life);

void AnimCurveExecuteRotate(u8* pCmdList, f32* pTarget, ulong tick, u16 seed,
                            ulong life);

void AnimCurveExecuteTexture(u8* pCmdList, Particle* pParticle, ulong tick,
                             u16 seed, ulong life);

void AnimCurveExecuteChild(u8* pCmdList, Particle* pParticle, ulong tick,
                           u16 seed, ulong life);

} // namespace ef
} // namespace nw4r

#endif
