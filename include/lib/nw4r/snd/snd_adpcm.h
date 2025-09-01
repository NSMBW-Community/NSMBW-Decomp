#ifndef NW4R_SND_ADPCM_H
#define NW4R_SND_ADPCM_H
#include <nw4r/types_nw4r.h>

#include <revolution/AX.h>

namespace nw4r {
namespace snd {
namespace detail {

s16 DecodeDspAdpcm(AXPBADPCM* pAdpcm, u8 bits);

} // namespace detail
} // namespace snd
} // namespace nw4r

#endif
