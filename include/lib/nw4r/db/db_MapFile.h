#pragma once

#include <nw4r/types_nw4r.h>
#include <nw4r/ut.h>
#include <lib/revolution/OS/OSLink.h>

namespace nw4r {
namespace db {

class MapFile;

MapFile *MapFile_RegistOnDvd(void *, const char *, const OSModuleInfo *);
void MapFile_Unregist(MapFile *);

} // namespace db
} // namespace nw4r
